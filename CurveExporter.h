// CurveExporter.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Animation/AnimSequence.h"   
#include "CurveExporter.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class YOUR_PROJECT_NAME_API UCurveExporter : public UActorComponent
{
    GENERATED_BODY()

public:
    UCurveExporter();

    // Pick your animation in the Details panel
    UPROPERTY(EditAnywhere, Category = "Curve Export")
    UAnimSequence* SequenceToExport;

    // This becomes the button you click in-editor
    UFUNCTION(CallInEditor, Category = "Curve Export")
    void ExportCurves();
};
