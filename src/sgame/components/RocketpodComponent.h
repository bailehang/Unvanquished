#ifndef ROCKETPOD_COMPONENT_H_
#define ROCKETPOD_COMPONENT_H_

#include "../backend/CBSEBackend.h"
#include "../backend/CBSEComponents.h"

class RocketpodComponent: public RocketpodComponentBase {
	public:
		// ///////////////////// //
		// Autogenerated Members //
		// ///////////////////// //

		/**
		 * @brief Default constructor of the RocketpodComponent.
		 * @param entity The entity that owns the component instance.
		 * @param r_HumanBuildableComponent A HumanBuildableComponent instance that this component depends on.
		 * @param r_TurretComponent A TurretComponent instance that this component depends on.
		 * @note This method is an interface for autogenerated code, do not modify its signature.
		 */
		RocketpodComponent(Entity& entity, HumanBuildableComponent& r_HumanBuildableComponent, TurretComponent& r_TurretComponent);

		/**
		 * @brief Handle the PrepareNetCode message.
		 * @note This method is an interface for autogenerated code, do not modify its signature.
		 */
		void HandlePrepareNetCode();

		/**
		 * @brief Handle the PowerUp message.
		 * @note This method is an interface for autogenerated code, do not modify its signature.
		 */
		void HandlePowerUp();

		/**
		 * @brief Handle the PowerDown message.
		 * @note This method is an interface for autogenerated code, do not modify its signature.
		 */
		void HandlePowerDown();

		/**
		 * @brief Handle the Die message.
		 * @param killer
		 * @param meansOfDeath
		 * @note This method is an interface for autogenerated code, do not modify its signature.
		 */
		void HandleDie(gentity_t* killer, meansOfDeath_t meansOfDeath);

		// ///////////////////// //

		// TODO: Make protected and add the rocket pod missile's component as a friend class.
		static bool SafeShot(int passEntityNumber, const Vec3& origin, const Vec3& direction);

	private:

		void Think(int timeDelta);

		bool CompareTargets(Entity& a, Entity& b);

		/**
		 * @return Whether an enemy is so close that hitting it with a rocket would harm the pod.
		 */
		bool EnemyClose();

		/**
		 * @return Whether an unguided rocket shot would hit friendly targets.
		 */
		bool SafeShot();

		void Shoot();

		/**
		 * @brief In safe mode, the shutters are closed and the rocketpod does not act.
		 */
		void SetSafeMode(bool on);

		bool firing;
		bool lockingOn;
		bool lockedOn;
		bool safeMode;

		int lastTargetSearch;
		int openingShuttersSince;
		int lockingOnSince;
		int lastShot;
};

#endif // ROCKETPOD_COMPONENT_H_
