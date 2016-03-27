// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ShipMetaball.generated.h"

UCLASS()
class VRRACER_API AShipMetaball : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShipMetaball();

    virtual void PostInitializeComponents() override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;


    UFUNCTION(BlueprintCallable, Category = "Metaball Sim")
    void AddBallForce(FVector worldForce);

    UFUNCTION(BlueprintCallable, Category = "Metaball Sim")
    void ResetVelocities();

	
    UStaticMeshComponent *MyRenderBox, *MyGlassContainer;

    UMaterialInstanceDynamic *MetaballMat;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Metaball Sim")
    TArray<float> BallRadii;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Metaball Sim")
    TArray<FVector> BallPoses;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Metaball Sim")
    TArray<FVector> BallVelocities;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metaball Sim")
    float VelocityScale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metaball Sim")
    float BallPushback;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metaball Sim")
    float BallPushbackRadiusScale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metaball Rendering")
    float MinEmissive;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metaball Rendering")
    float MaxEmissive;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Metaball Rendering")
    float CurrentEmissive;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metaball Rendering")
    float Thinness;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metaball Rendering")
    float RadiusScale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metaball Rendering")
    float Increment;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metaball Rendering")
    FColor VoidColor;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metaball Rendering")
    UTexture2D *VoidTex;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metaball Rendering")
    float VoidTexScale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metaball Rendering")
    FColor BallColor;

    UPROPERTY(BlueprintReadWrite, Category = "Metaball Rendering")
    UAudioComponent* GameMusic;
    UPROPERTY(BlueprintReadWrite, Category = "Metaball Rendering")
    float TimeSinceMusicStart;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
    int32 UpdateVels;


    //TODO: Music.

private:

    void UpdateRenderingStuff(bool updateTex);
    void UpdateBallPhysics(float deltaTime);
    void UpdateMusicStuff(float deltaTime);


    FVector localBallForce;
    TArray<float> musicSamples;
};
