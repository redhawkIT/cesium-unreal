// Copyright 2020-2021 CesiumGS, Inc. and Contributors

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"

#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include "GlobeAwareDefaultPawn.generated.h"

class ACesiumGeoreference;
class UCurveFloat;

/**
 * This pawn can be used to easily move around the globe while maintaining a
 * sensible orientation. As the pawn moves across the horizon, it automatically
 * changes its own up direction such that the world always looks right-side up.
 */
UCLASS()
class CESIUMRUNTIME_API AGlobeAwareDefaultPawn : public ADefaultPawn {
  GENERATED_BODY()

  AGlobeAwareDefaultPawn();

  /**
   * The actor controlling how this camera's location in the Cesium world
   * relates to the coordinate system in this Unreal Engine level.
   */
  UPROPERTY(EditAnywhere, Category = "Cesium")
  ACesiumGeoreference* Georeference;

  /**
   * Input callback to move forward in local space (or backward if Val is
   * negative).
   * @param Val Amount of movement in the forward direction (or backward if
   * negative).
   * @see APawn::AddMovementInput()
   */
  virtual void MoveForward(float Val) override;

  /**
   * Input callback to strafe right in local space (or left if Val is negative).
   * @param Val Amount of movement in the right direction (or left if negative).
   * @see APawn::AddMovementInput()
   */
  virtual void MoveRight(float Val) override;

  /**
   * Input callback to move up in world space (or down if Val is negative).
   * @param Val Amount of movement in the world up direction (or down if
   * negative).
   * @see APawn::AddMovementInput()
   */
  virtual void MoveUp_World(float Val) override;

  /**
   * Called via input to turn at a given rate.
   * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired
   * turn rate
   */
  virtual void TurnAtRate(float Rate) override;

  /**
   * Called via input to look up at a given rate (or down if Rate is negative).
   * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired
   * turn rate
   */
  virtual void LookUpAtRate(float Rate) override;

  virtual void AddControllerPitchInput(float Val) override;
  virtual void AddControllerYawInput(float Val) override;
  virtual void AddControllerRollInput(float Val) override;
  virtual FRotator GetViewRotation() const override;
  virtual FRotator GetBaseAimRotation() const override;

public:
  // Useful transformations

  /**
   * Transforms a rotator expressed in UE coordinates to one expressed in ENU
   * coordinates. (Single precision, but this should not be an issue)
   */
  FRotator TransformRotatorUEToENU(FRotator UERotator);

  /**
   * Transforms a rotator expressed in ENU coordinates to one expressed in UE
   * coordinates. (Single precision, but this should not be an issue)
   */
  FRotator TransformRotatorENUToUE(FRotator ENURotator);

  /**
   * Get the pawn Camera location accurately in ECEF Coordinates
   */
  glm::dvec3 GetECEFCameraLocation();
  
  /**
   * Set the pawn Camera location accurately from ECEF Coordinates
   */
  void SetECEFCameraLocation(glm::dvec3 ECEF);

  /** 
   * This curve dictates what percentage of the max altitude the pawn should
   * take at a given time on the curve. This curve must be kept in the 0 to
   * 1 range on both axes. The {@see FlyToMaximumAltitudeCurve} dictates the
   * actual max altitude at each point along the curve.
   */ 
  UPROPERTY(EditAnywhere, Category = "Cesium")
  UCurveFloat* FlyToAltitudeProfileCurve;

  /**
   * This curve is used to determine the progress percentage for all the other
   * curves. This allows us to accelerate and deaccelerate as wanted throughout
   * the curve.
   */
  UPROPERTY(EditAnywhere, Category = "Cesium")
  UCurveFloat* FlyToProgressCurve;

  /**
   * This curve dictates the maximum altitude at each point along the curve.
   * This can be used in conjunction with the {@see FlyToAltitudeProfileCurve}
   * to allow the pawn to take some altitude during the flight.
   */
  UPROPERTY(EditAnywhere, Category = "Cesium")
  UCurveFloat* FlyToMaximumAltitudeCurve;

  /**
   * The length in seconds that the flight should last.
   */
  UPROPERTY(EditAnywhere, Category = "Cesium")
  double FlyToDuration = 5;

  /**
   * The granularity in degrees with which keypoints should be generated for
   * the flight interpolation.
   */
  UPROPERTY(EditAnywhere, Category = "Cesium")
  double FlyToGranularityDegrees = 0.01;

  /**
   * Begin a smooth camera flight to the specified destination ECEF such that
   * the camera ends at the specified yaw and pitch. The characteristics of the
   * flight can be configured with {@see FlyToAltitudeProfileCurve}, 
   * {@see FlyToProgressCurve}, {@see FlyToMaximumAltitudeCurve}, 
   * {@see FlyToDuration}, and {@see FlyToGranularityDegrees}.
   */
  void FlyToLocationECEF(
      glm::dvec3 ECEFDestination,
      float YawAtDestination,
      float PitchAtDestination);

  /**
   * Begin a smooth camera flight to the specified destination ECEF such that
   * the camera ends at the specified yaw and pitch. The characteristics of the
   * flight can be configured with {@see FlyToAltitudeProfileCurve},
   * {@see FlyToProgressCurve}, {@see FlyToMaximumAltitudeCurve},
   * {@see FlyToDuration}, and {@see FlyToGranularityDegrees}.
   */
  UFUNCTION(BlueprintCallable)
  void InaccurateFlyToLocationECEF(
      FVector ECEFDestination,
      float YawAtDestination,
      float PitchAtDestination);

  /**
   * Begin a smooth camera flight to the specified destination LLH such that
   * the camera ends at the specified yaw and pitch. The characteristics of the
   * flight can be configured with {@see FlyToAltitudeProfileCurve}, 
   * {@see FlyToProgressCurve}, {@see FlyToMaximumAltitudeCurve}, 
   * {@see FlyToDuration}, and {@see FlyToGranularityDegrees}.
   */
  void FlyToLocationLongLatHeight(
      glm::dvec3 LongLatHeightDestination,
      float YawAtDestination,
      float PitchAtDestination);

  /**
   * Begin a smooth camera flight to the specified destination LLH such that
   * the camera ends at the specified yaw and pitch. The characteristics of the
   * flight can be configured with {@see FlyToAltitudeProfileCurve},
   * {@see FlyToProgressCurve}, {@see FlyToMaximumAltitudeCurve},
   * {@see FlyToDuration}, and {@see FlyToGranularityDegrees}.
   */
  UFUNCTION(BlueprintCallable)
  void InaccurateFlyToLocationLongLatHeight(
      FVector LongLatHeightDestination,
      float YawAtDestination,
      float PitchAtDestination);

  virtual void Tick(float DeltaSeconds) override;

protected:
  virtual void BeginPlay() override;

private:
  /**
   * Computes the local East-North-Up to Fixed frame transformation based on the
   * current
   * {@see GetPawnViewLocation}. The returned transformation works in Unreal's
   * left-handed coordinate system.
   */
  glm::dmat3 _computeEastNorthUpToFixedFrame() const;

  // helper variables for FlyToLocation
  bool _bFlyingToLocation = false;
  double _currentFlyTime = 0.0;
  FRotator _flyToSourceRotation;
  FRotator _flyToDestinationRotation;

  std::vector<glm::dvec3> _keypoints;
};
