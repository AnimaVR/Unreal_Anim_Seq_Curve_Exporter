// CurveExporter.cpp
#include "CurveExporter.h"
#include "Misc/FileHelper.h"        
#include "Misc/Paths.h"             
#include "HAL/FileManager.h"           
#include "Misc/DateTime.h"            
#include "Animation/AnimCurveTypes.h"  
#include "Engine/Engine.h"            

UCurveExporter::UCurveExporter()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UCurveExporter::ExportCurves()
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green,
            TEXT("CurveExporter: ExportCurves() pressed!"));
    }

    if (!SequenceToExport)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Curve Export] No AnimSequence assigned!"));
        return;
    }

    // Build timestamped folder under C:/Temp
    FDateTime Now = FDateTime::Now();
    FString Timestamp = Now.ToString(TEXT("%Y%m%d_%H%M%S"));
    FString BaseTemp = TEXT("C:/Temp");
    FString OutputDir = FPaths::Combine(BaseTemp, Timestamp);
    IFileManager::Get().MakeDirectory(*OutputDir, /*Tree=*/true);

    FString FileName = SequenceToExport->GetName() + TEXT(".csv");
    FString FullPath = FPaths::Combine(OutputDir, FileName);

    UE_LOG(LogTemp, Log, TEXT("[Curve Export] Writing to: %s"), *FullPath);

    const FRawCurveTracks& Curves = SequenceToExport->GetCurveData();
    if (Curves.FloatCurves.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Curve Export] No float curves found."));
        return;
    }

    // Collect & sort curve names
    TArray<FName> Names;
    for (const FFloatCurve& C : Curves.FloatCurves)
        Names.Add(C.GetName());
    Names.Sort([](const FName& A, const FName& B) { return A.ToString() < B.ToString(); });

    // Map for fast lookup
    TMap<FName, const FFloatCurve*> CurveMap;
    for (const FFloatCurve& C : Curves.FloatCurves)
        CurveMap.Add(C.GetName(), &C);

    // Build CSV rows
    TArray<FString> CSVRows;
    {
        FString Header = TEXT("Frame");
        for (auto& N : Names) Header += TEXT(",") + N.ToString();
        CSVRows.Add(Header);
    }

    const float SampleRate = 60.f;
    const float Length = SequenceToExport->GetPlayLength();
    const int32 NumFrames = FMath::CeilToInt(Length * SampleRate) + 1;

    for (int32 Frame = 0; Frame < NumFrames; ++Frame)
    {
        float Time = Frame / SampleRate;
        FString Line = FString::Printf(TEXT("%d"), Frame);

        for (auto& N : Names)
        {
            float Val = 0.f;
            if (const FFloatCurve* C = CurveMap.FindRef(N))
            {
                Val = C->Evaluate(Time);
            }
            Line += FString::Printf(TEXT(",%f"), Val);
        }
        CSVRows.Add(Line);
    }

    // Save the file
    if (FFileHelper::SaveStringArrayToFile(CSVRows, *FullPath))
    {
        UE_LOG(LogTemp, Log, TEXT("[Curve Export] Successfully saved CSV."));
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1, 4.f, FColor::Cyan,
                FString::Printf(TEXT("Exported to %s"), *FullPath)
            );
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[Curve Export] Failed to save CSV!"));
    }
}
