// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SneakyHUD.h"

#include "Blueprint/UserWidget.h"
#include "UI/MainPlayerOverlay.h"

void ASneakyHUD::DrawHUD()
{
	Super::DrawHUD();

	if(GEngine)
	{
		FVector2D ViewportSize;
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

		const float SpreadScaled = CrosshairSpreadMax * HUDCrosshairStruct.CrosshairSpread;
		
		if(HUDCrosshairStruct.CrosshairsCenter)
		{
			const FVector2D Spread(0.f, 0.f);
			DrawCrossHair(HUDCrosshairStruct.CrosshairsCenter, ViewportCenter, Spread, HUDCrosshairStruct.Crosshairscolor);
		}
		if(HUDCrosshairStruct.CrosshairsLeft)
		{
			const FVector2D Spread(-SpreadScaled, 0.f);
			DrawCrossHair(HUDCrosshairStruct.CrosshairsLeft, ViewportCenter, Spread, HUDCrosshairStruct.Crosshairscolor);
		}
		if(HUDCrosshairStruct.CrosshairsRight)
		{
			const FVector2D Spread(SpreadScaled, 0.f);
			DrawCrossHair(HUDCrosshairStruct.CrosshairsRight, ViewportCenter, Spread, HUDCrosshairStruct.Crosshairscolor);
		}
		if(HUDCrosshairStruct.CrosshairsBottom)
		{
			const FVector2D Spread(0.f, -SpreadScaled);
			DrawCrossHair(HUDCrosshairStruct.CrosshairsBottom, ViewportCenter, Spread, HUDCrosshairStruct.Crosshairscolor);
		}
		if(HUDCrosshairStruct.CrosshairsTop)
		{
			const FVector2D Spread(0.f, SpreadScaled);
			DrawCrossHair(HUDCrosshairStruct.CrosshairsTop, ViewportCenter, Spread, HUDCrosshairStruct.Crosshairscolor);
		}
	}
}

void ASneakyHUD::BeginPlay()
{
	Super::BeginPlay();

}

void ASneakyHUD::AddCharacterOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if(PlayerController && IsValid(MainPlayerOverlayClass))
	{
		MainPlayerOverlayRef = CreateWidget<UMainPlayerOverlay> (PlayerController, MainPlayerOverlayClass);
		MainPlayerOverlayRef->AddToViewport();
	}
}

void ASneakyHUD::DrawCrossHair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairColor)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D TextureDrawPoint(ViewportCenter.X - (TextureWidth / 2.f) + Spread.X,
	ViewportCenter.Y - (TextureHeight / 2.f + Spread.Y));

	DrawTexture(Texture, TextureDrawPoint.X,
		TextureDrawPoint.Y, TextureWidth, TextureHeight, 0.f, 0.f, 1.f,
		1.f, CrosshairColor);
	
}

