#pragma once

#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include<glm/gtx/string_cast.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
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
		vec3 Step(float dt, PlaneCollider * pc) {
			newtonianAttributeSet.Force += newtonianAttributeSet.Mass * vec3(0,-2,0);// Force of gravity
			vec3 frictionForce = (-newtonianAttributeSet.Velocity) / dt;
			newtonianAttributeSet.Force += 0.01f * frictionForce;// Force of friction

			if (newtonianAttributeSet.Position.y <= 2.0f) {
				vec3 stoppingForce = (-newtonianAttributeSet.Velocity) / dt;
				newtonianAttributeSet.Force.y += stoppingForce.y;
			}
			newtonianAttributeSet.Velocity += newtonianAttributeSet.Force / newtonianAttributeSet.Mass * dt;
			newtonianAttributeSet.Position += newtonianAttributeSet.Velocity;

			vec3 preResetForce = newtonianAttributeSet.Force;

			newtonianAttributeSet.Force = vec3(0); // Reset net force
			return preResetForce;
		}
	};


};

