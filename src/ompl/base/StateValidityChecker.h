/*********************************************************************
* Software License Agreement (BSD License)
*
*  Copyright (c) 2008, Willow Garage, Inc.
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the Willow Garage nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/

/* Author: Ioan Sucan */

#ifndef OMPL_BASE_STATE_VALIDITY_CHECKER_
#define OMPL_BASE_STATE_VALIDITY_CHECKER_

#include "ompl/base/State.h"
#include "ompl/util/ClassForward.h"

namespace ompl
{

    namespace base
    {

        /// @cond IGNORE
        ClassForward(SpaceInformation);
        /// @endcond

        /// @cond IGNORE
        /** \brief Forward declaration of ompl::base::StateValidityChecker */
        ClassForward(StateValidityChecker);
        /// @endcond

        /** \class ompl::base::StateValidityCheckerPtr
            \brief A boost shared pointer wrapper for ompl::base::StateValidityChecker */

        /** \brief Properties that a state validity checker may have */
        struct StateValidityCheckerSpecs
        {
            StateValidityCheckerSpecs(void) : hasExactClearance(false), hasBoundedApproximateClearance(false), hasApproximateClearance(false), hasGradientComputation(false)
            {
            }

            /** \brief Flag indicating whether the StateValidityChecker can
                compute the exact clearance of a state */
            bool hasExactClearance;

            /** \brief Flag indicating that the clearance value computed by the
                StateValidityChecker is approximated by a lower bound */
            bool hasBoundedApproximateClearance;

            /** \brief Flag indicating that the clearance value computed by the
                StateValidityChecker is a conservative approximation */
            bool hasApproximateClearance;

            /** \brief Flag indicating that this state validity checker can return
                a direction that moves a state away from being invalid. */
            bool hasGradientComputation;
        };

        /** \brief Abstract definition for a class checking the
            validity of states. The implementation of this class must
            be thread safe. */
        class StateValidityChecker
        {
        public:

            /** \brief Constructor */
            StateValidityChecker(SpaceInformation* si) : si_(si)
            {
            }

            /** \brief Constructor */
            StateValidityChecker(const SpaceInformationPtr &si) : si_(si.get())
            {
            }

            virtual ~StateValidityChecker(void)
            {
            }

            /** \brief Return true if the state \e state is valid. Usually, this means at least collision checking. If it is
                possible that ompl::base::StateSpace::interpolate() or ompl::control::ControlSpace::propagate() return states that
                are outside of bounds, this function should also make a call to ompl::base::SpaceInformation::satisfiesBounds(). */
            virtual bool isValid(const State *state) const = 0;

            /** \brief Return true if the state \e state is valid. In addition, set \e dist to the distance to the nearest invalid state. */
            virtual bool isValid(const State *state, double &dist) const
            {
                dist = clearance(state);
                return isValid(state);
            }

            /** \brief Return true if the state \e state is valid. In addition, set \e dist to the distance to the nearest invalid state.
                If a direction that moves \e state away from being invalid is available, it is set in \e gradient. \e gradient is an element
                of the tangent space that contains \e state.  \e gradientAvailable is set to true if \e gradient is updated. */
            virtual bool isValid(const State *state, double &dist, State *gradient, bool &gradientAvailable) const
            {
                dist = clearance(state, gradient, gradientAvailable);
                return isValid(state);
            }

            /** \brief Report the distance to the nearest invalid state when starting from \e state. If the distance is
                negative, the value of clearance is the penetration depth.*/
            virtual double clearance(const State *state) const
            {
                return 0.0;
            }

            /** \brief Report the distance to the nearest invalid state when starting from \e state, and if available,
                also set the gradient: the direction that moves away from the colliding state. \e gradientAvailable is set
                to true if \e gradient is updated. */
            virtual double clearance(const State *state, State *gradient, bool &gradientAvailable) const
            {
                gradientAvailable = false;
                return clearance(state);
            }

            /** \brief Return the specifications (capabilities of this state validity checker) */
            const StateValidityCheckerSpecs& getSpecs(void) const
            {
                return specs_;
            }

        protected:

            /** \brief The instance of space information this state validity checker operates on */
            SpaceInformation*         si_;

            /** \brief The specifications of the state validity checker (its capabilities) */
            StateValidityCheckerSpecs specs_;

        };

        /** \brief The simplest state validity checker: all states are valid */
        class AllValidStateValidityChecker : public StateValidityChecker
        {
        public:

            /** \brief Constructor */
            AllValidStateValidityChecker(SpaceInformation* si) : StateValidityChecker(si)
            {
            }

            /** \brief Constructor */
            AllValidStateValidityChecker(const SpaceInformationPtr &si) : StateValidityChecker(si)
            {
            }

            /** \brief Always return true (all states are considered valid) */
            virtual bool isValid(const State * /* state */ ) const
            {
                return true;
            }
        };

    }
}

#endif
