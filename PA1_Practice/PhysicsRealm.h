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


		

		vec3 Step(float dt, PlaneCollider * pc, bool handleCollision, vec3 normal) {
			newtonianAttributeSet.Force += newtonianAttributeSet.Mass * vec3(0,-.9810,0);// Force of gravity
			vec3 frictionForce = (-newtonianAttributeSet.Velocity) / dt;
			newtonianAttributeSet.Force += frictionForce*.01f;
			
			newtonianAttributeSet.Velocity += newtonianAttributeSet.Force / newtonianAttributeSet.Mass * dt;
			
			if (handleCollision) {
				// Collision rebounds can never cause increase in velocity

				float velocityNormalDotProduct = dot(newtonianAttributeSet.Velocity, normal);

				vec3 forceAlongTheSurfaceNormal = -length(newtonianAttributeSet.Velocity) * normal;
				vec3 stoppingForce = (-forceAlongTheSurfaceNormal) / dt;
				float preVol = length(newtonianAttributeSet.Velocity);
				// Then we apply it
				newtonianAttributeSet.Velocity += stoppingForce / newtonianAttributeSet.Mass * dt;
				if (length(newtonianAttributeSet.Velocity) > preVol) newtonianAttributeSet.Velocity = normalize(newtonianAttributeSet.Velocity) * preVol;


			}
			

			
			
			newtonianAttributeSet.Position += newtonianAttributeSet.Velocity;




			vec3 preResetForce = newtonianAttributeSet.Force;

			newtonianAttributeSet.Force = vec3(0); // Reset net force
			return preResetForce;
		}
	};


};

