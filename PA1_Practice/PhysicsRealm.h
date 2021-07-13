#pragma once

#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include<glm/gtx/string_cast.hpp>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "PhysicsCommon.h"

using namespace glm;
static class PhysicsRealm
{
public:
	vec3 gravity = vec3(0, -9.81, 0);

	class Collider {

	}; 
	
	class PlaneCollider : public Collider {
	public:
		vec3 Normal, ContainingPoint;
		PlaneCollider(vec3 Normal, vec3 ContainingPoint) { this->Normal = Normal; this->ContainingPoint = ContainingPoint; }
		float DistanceFromPoint(vec3 Point) {
			vec3 v = Point - ContainingPoint;
			return v.x*Normal.x + v.y*Normal.y + v.z*Normal.z;
		}

	};

	class SphereCollider : public Collider {
	public:
		vec3 Center; // Center
		float Radius; // Radius

		SphereCollider(vec3 Center, float Radius) { this->Center = Center, this->Radius = Radius; }
		bool CollidingWithPlane(PlaneCollider* planeCollider) {
			return DistanceFromPlane(planeCollider) <= Radius;
		}

		float DistanceFromPlane(PlaneCollider* planeCollider) {
			vec3 v = Center - planeCollider->ContainingPoint;
			return v.x * planeCollider->Normal.x + v.y * planeCollider->Normal.y + v.z * planeCollider->Normal.z;
		}
	};

	
	class NewtonianAttributeSet {
	public:
		vec3 Position, Velocity, Force;
		float Mass=1.0f;

	};

	class EuclidianObject {
	public:
		Collider * collider;
		NewtonianAttributeSet newtonianAttributeSet;
		EuclidianObject(Collider* collider, NewtonianAttributeSet newtonianAttributeSet) { this->collider = collider, this->newtonianAttributeSet = newtonianAttributeSet; }
		float DistanceFromPlane(PlaneCollider* planeCollider) {
			vec3 v = newtonianAttributeSet.Position - planeCollider->ContainingPoint;
			return v.x * planeCollider->Normal.x + v.y * planeCollider->Normal.y + v.z * planeCollider->Normal.z;
		}
		bool CollidingWithPlane(PlaneCollider* planeCollider) {
			return (DistanceFromPlane(planeCollider)) <= 2.0f;
		}


		

		vec3 Force_Integration_Step(float dt) {

			// Apply unavoidable forces here.
			newtonianAttributeSet.Force += newtonianAttributeSet.Mass * vec3(0, - FORCE_OF_GRAVITY, 0);// Force of gravity
			// Forces due to collision resolution should probably be solved here .



			// INCREMENT POSITION BY VELOCITY X DT
			newtonianAttributeSet.Position += newtonianAttributeSet.Velocity;


			// Calculate acceleration due to net forces
			vec3 acceleration(0.0f);

			// F = ma; a = F/m
			acceleration = newtonianAttributeSet.Force / newtonianAttributeSet.Mass;

			// Update velocity due to acceleration;
			newtonianAttributeSet.Velocity += acceleration * dt;

			// Impose drag or other relevant 'after-effect' forces
			newtonianAttributeSet.Velocity *= (1.0f - AIR_DRAG); // -0.5% reduction due to drag.
		
			// Reset net force?
			newtonianAttributeSet.Force = vec3(0); // Reset net force

			vec3 preResetForce = newtonianAttributeSet.Force;
			return preResetForce;
		}
	};


};

