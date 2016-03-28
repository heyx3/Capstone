// Fill out your copyright notice in the Description page of Project Settings.

#include "VRRacer.h"

#include <string>

#include "ShipMetaball.h"


namespace
{
    const int32 N_BALLS = 5;
}


// Sets default values
AShipMetaball::AShipMetaball()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


    //Default values.

    localBallForce = FVector::ZeroVector;

    VelocityScale = 1.0f;

    TimeSinceMusicStart = 0.0f;

    BallPushback = 2.0f;
    BallPushbackRadiusScale = 0.5f;

    MinEmissive = 0.5f;
    MaxEmissive = 1.0f;
    Thinness = 100.0f;
    Increment = 8.0f;
    RadiusScale = 1.0f;
    CurrentEmissive = 0.5f;
    VoidColor = FColor::White;
    BallColor = FColor::White;
    Hack_PosOffset = FVector(0.0f, 0.0f, 0.0f);

    UpdateVels = 0;

    static ConstructorHelpers::FObjectFinder<UTexture2D> voidTex(TEXT("/Game/StarterContent/TExtures/T_Concrete_Grime_D.T_Concrete_Grime_D"));
    if (voidTex.Succeeded())
    {
        VoidTex = voidTex.Object;
    }

    VoidTexScale = 1.0f;


    //Set up components.

    USceneComponent* rootSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    RootComponent = rootSceneComp;

    MyRenderBox = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Metaball Box"));
    MyGlassContainer = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Glass Container"));

    MyRenderBox->AttachTo(RootComponent);
    MyGlassContainer->AttachTo(MyRenderBox);

    //Load the box mesh for both components.
    static ConstructorHelpers::FObjectFinder<UStaticMesh> unitBoxMesh(TEXT("/Game/My_Content/Meshes/UnitBox.UnitBox"));
    if (unitBoxMesh.Succeeded())
    {
        MyRenderBox->SetStaticMesh(unitBoxMesh.Object);
        MyRenderBox->SetRelativeLocation(FVector(0.0f));
        MyRenderBox->SetWorldScale3D(FVector(1.0f));
        MyRenderBox->bGenerateOverlapEvents = 0;
        MyRenderBox->SetSimulatePhysics(false);
        MyRenderBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        

        MyGlassContainer->SetStaticMesh(unitBoxMesh.Object);
        MyGlassContainer->SetRelativeLocation(FVector(0.0f));
        MyGlassContainer->SetWorldScale3D(FVector(1.1f));
        MyGlassContainer->bGenerateOverlapEvents = 0;
        MyGlassContainer->SetSimulatePhysics(false);
        MyGlassContainer->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        static ConstructorHelpers::FObjectFinder<UMaterial> glassMat(TEXT("/Game/My_Content/Materials/MetaballContainerMat.MetaballContainerMat"));
        if (glassMat.Succeeded())
        {
            MyGlassContainer->SetMaterial(0, glassMat.Object);
        }
    }


    //Generate starting data for the balls.
    BallPoses.Empty();
    BallRadii.Empty();
    BallVelocities.Empty();
    for (int i = 0; i < N_BALLS; ++i)
    {
        BallPoses.Add(FMath::RandPointInBox(FBox(FVector(-1.0f), FVector(1.0f))));
        BallVelocities.Add(FMath::VRand());
        BallRadii.Add(0.25f);
    }

    //Find the metaball material.
    static ConstructorHelpers::FObjectFinder<UMaterial> mat(TEXT("Material'/Game/My_Content/Materials/MetaballMat.MetaballMat'"));
    if (mat.Succeeded())
    {
        baseMetaballMat = mat.Object;
    }
    else
    {
        baseMetaballMat = nullptr;
    }
}
void AShipMetaball::PostInitializeComponents()
{
    Super::PostInitializeComponents();
}

// Called when the game starts or when spawned
void AShipMetaball::BeginPlay()
{
	Super::BeginPlay();

    //Create a dynamic version of the metaball material.
    if (baseMetaballMat != nullptr)
    {
        MetaballMat = UMaterialInstanceDynamic::Create(baseMetaballMat, this);
        MyRenderBox->SetMaterial(0, MetaballMat);
        UpdateRenderingStuff(false);
    }
    else
    {
        MetaballMat = nullptr;
    }
}

void AShipMetaball::AddBallForce(FVector worldForce)
{
    localBallForce += GetTransform().InverseTransformVector(worldForce);
}
void AShipMetaball::ResetVelocities()
{
    for (int i = 0; i < N_BALLS; ++i)
        BallVelocities[i] = FMath::VRand();
}

// Called every frame
void AShipMetaball::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (UpdateVels != 0)
    {
        UpdateVels = 0;
        ResetVelocities();
    }

    UpdateRenderingStuff(true);
    UpdateBallPhysics(DeltaTime);

    CurrentEmissive = FMath::Lerp(MinEmissive, MaxEmissive, GetCurrentEmissiveLerp(DeltaTime));
}
void AShipMetaball::UpdateBallPhysics(float DeltaTime)
{
    //Update ball physics.
    for (int32 i = 0; i < N_BALLS; ++i)
    {
        FVector& ballVel = BallVelocities[i];
        FVector& ballPos = BallPoses[i];
        float& ballRad = BallRadii[i];
        float scaledRad = BallPushbackRadiusScale * ballRad;
        float invRad = 1.0f / scaledRad;

        //Apply force to the ball.
        ballVel += localBallForce * DeltaTime;

        //If it's being crushed against the edge of the bounds, push back.
        float p;
        float temp = BallPushback * invRad;
        p = ballPos.X - scaledRad;
        if (p < -1.0f)
            ballVel.X -= temp * (p + 1.0f);
        p = ballPos.X + scaledRad;
        if (p > 1.0f)
            ballVel.X -= temp * (p - 1.0f);
        p = ballPos.Y - scaledRad;
        if (p < -1.0f)
            ballVel.Y -= temp * (p + 1.0f);
        p = ballPos.Y + scaledRad;
        if (p > 1.0f)
            ballVel.Y -= temp * (p - 1.0f);
        p = ballPos.Z - scaledRad;
        if (p < -1.0f)
            ballVel.Z -= temp * (p + 1.0f);
        p = ballPos.Z + scaledRad;
        if (p > 1.0f)
            ballVel.Z -= temp * (p - 1.0f);
           
        //Push the balls away from each other.
        for (int32 j = i + 1; j < N_BALLS; ++j)
        {
            float jRad = BallRadii[j] * BallPushbackRadiusScale;
            float distSqr = (scaledRad + jRad);
            distSqr *= distSqr;

            if (FVector::DistSquared(ballPos, BallPoses[j]))
            {
                FVector forceForI = DeltaTime * BallPushback * (ballPos - BallPoses[j]);
                ballVel += forceForI * invRad;
                BallVelocities[j] -= forceForI / jRad;
            }
        }

        //Apply velocity.
        ballPos += ballVel * VelocityScale * DeltaTime;

        //If the ball is outside the edge of the bounds, snap back.
        if (ballPos.X < -1.0f)
        {
            ballPos.X = -1.0f;
            ballVel.X = FMath::Max(0.0f, ballVel.X);
        }
        if (ballPos.X > 1.0f)
        {
            ballPos.X = 1.0f;
            ballVel.X = FMath::Min(0.0f, ballVel.X);
        }
        if (ballPos.Y < -1.0f)
        {
            ballPos.Y = -1.0f;
            ballVel.Y = FMath::Max(0.0f, ballVel.Y);
        }
        if (ballPos.Y > 1.0f)
        {
            ballPos.Y = 1.0f;
            ballVel.Y = FMath::Min(0.0f, ballVel.Y);
        }
        if (ballPos.Z < -1.0f)
        {
            ballPos.Z = -1.0f;
            ballVel.Z = FMath::Max(0.0f, ballVel.Z);
        }
        if (ballPos.Z > 1.0f)
        {
            ballPos.Z = 1.0f;
            ballVel.Z = FMath::Min(0.0f, ballVel.Z);
        }
    }

    localBallForce = FVector::ZeroVector;
}

void AShipMetaball::UpdateRenderingStuff(bool updateTex)
{
    MetaballMat = Cast<UMaterialInstanceDynamic>(MyRenderBox->GetMaterial(0));
    if (MetaballMat == nullptr)
        return;

    MetaballMat->SetScalarParameterValue(TEXT("Emissive Brightness"), CurrentEmissive);
    MetaballMat->SetScalarParameterValue(TEXT("Threshold"), Thinness);
    MetaballMat->SetScalarParameterValue(TEXT("Increment"), Increment);
    MetaballMat->SetVectorParameterValue(TEXT("Ball Color"), BallColor);
    MetaballMat->SetVectorParameterValue(TEXT("Void Color"), VoidColor);
    if(updateTex)
    {
        MetaballMat->SetTextureParameterValue(TEXT("Void Texture"), VoidTex);
    }
    MetaballMat->SetScalarParameterValue(TEXT("Void Texture Scale"), VoidTexScale);


    for (int32 i = 0; i < N_BALLS; ++i)
    {
        FVector pos = BallPoses[i];
        float radius = BallRadii[i] * RadiusScale;

        FString str = TEXT("Ball ");
        str.Append(FString::FromInt(i + 1));

        FString str2 = str;
        str2.Append(" Pos");
        MetaballMat->SetVectorParameterValue(FName(*str2), pos);

        str2 = str;
        str2.Append(" Radius");
        MetaballMat->SetScalarParameterValue(FName(*str2), radius);
    }
}