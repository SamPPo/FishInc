# FishInc - UE5 Project

## Project Overview
An incremental fishing game. The player owns a fleet of boats and ships that
fish automatically when positioned over fish schools. Caught fish generate
currency used to upgrade the fleet.

**Core game loop:**
Gameplay is divided into two loops:
The fishing loop is when the player moves their ships over fish schools in a lake area.
The village loop is when the player buys new buildings and upgrades existing ones in the village area.
These two loops are independent of each other, like two different states, but work in the same level.

Fishing loop in more detail:
Player starts the fishing loop →
Fish Schools spawn →
Player moves ships over them →
Fish are caught and accumulate in each ship's local storage (FishStoredAmount) →
When all the fish have been caught, or the storage is full, the player ends the loop.

Village loop in more detail:
The loop state transitions to Village →
All ships automatically transfer their stored fish to EconomySubsystem (ECurrencyType::Fish) →
The ship sails back to dock →
Player clicks on buildings to make processing happen →
Processes take fish from EconomySubsystem and convert it into coins →
Coins are stored in the EconomySubsystem →
Player uses currency to buy new buildings and upgrade existing ones →
When ready the player ends the village loop →
Player starts a new fishing loop.

Understanding this loop is required before adding any new system.
Every new feature must fit into this flow or explicitly justify why it does not.

**[PENDING] Fish School ownership and ship detection:**
Define who owns fish school spawning and how ships detect schools
(overlap volumes on ship, overlap on school, subsystem-managed, etc).
This must be decided by the lead before implementing any new ship behaviour
or fish school feature — agents must not guess at this.

## Key Systems
- **LoopStateSubsystem** — manages `ELoopState` (Fishing/Village). The authoritative gate for
  all loop transitions. Broadcasts `FOnLoopStateChanged` when state changes. All systems that
  care about loop state bind to this delegate; never poll the state on tick.
- **EconomySubsystem** — handles all currencies (Fish, Coins, Pearls). Fish enters here from
  ships on loop transition. All currency reads and writes go through this subsystem.
- **UpgradeRegistry** — pure stat aggregator. Buildings register precomputed `FStatContribution`
  records (stat tag, additive/multiplicative, value) via `RegisterContributions(InstanceID, ...)`.
  `ResolveStat(FGameplayTag)` sums all registered contributions for a tag. Fires `OnStatChanged`
  after every registration change so ships and workers can refresh cached values. No asset
  loading, no purchase logic — those live on `Building_Base`.
- **BuildingRegistry** — auto-loads all `UBuildingDefinition` assets via AssetManager on
  startup. Broadcasts `FOnBuildingDefinitionsLoaded` when ready. Drop an asset in
  `/Game/Data/Buildings/` and it is automatically available in the shop — no manual array to update.
- **BuildingDefinition** — `UPrimaryDataAsset` defining a purchasable building: display name,
  description, icon, purchase cost, building class to spawn, preview mesh. Also owns all upgrade
  data: `MaxUpgradeLevel`, `UpgradeCostBase`, `UpgradeCostGrowthFactor`, and a
  `TArray<FBuildingUpgrade> Upgrades`. Each `FBuildingUpgrade` entry defines one stat effect
  (tag, additive/multiplicative, base value, value-per-level) that scales with building level.
  `EUpgradeType` is defined in `UpgradeRegistry.h`; `FBuildingUpgrade` is defined in
  `BuildingDefinition.h`.
- **Fish_School** — fishable `InstancedStaticMeshComponent`.
- **Ship_Base** — base class for all ships that fish automatically.
  - 3D ship that the player can move around.
  - Fishes automatically with trace checks (only while `bInFishingLoop` is true).
  - Fish accumulates in a plain double buffer (`FishStoredAmount` / `FishStorageCapacity`).
  - Transfers stored fish to `EconomySubsystem` automatically when the Village loop begins,
    or manually via `TransferFishToEconomy()`.
  - Reads stats from `UpgradeRegistry::ResolveStat` on top of `ShipDefinition` base values.
    Refreshes cached stats when `OnStatChanged` fires for a relevant tag.
- **Building_Base** — base class for all buildings.
  - 3D placeable building spawned via deferred spawn from `FishIncPlayerController`.
  - `InitFromDefinition(UBuildingDefinition*)` must be called before `FinishSpawning`. It sets
    `Definition`, assigns a unique `BuildingInstanceID`, sets `BuildingLevel = 1`, and registers
    level-1 stat contributions with `UpgradeRegistry`.
  - `PurchaseUpgrade()` — spends coins, increments `BuildingLevel`, re-registers contributions.
  - `EndPlay` unregisters contributions from `UpgradeRegistry`.
  - Buildings are clickable (no click rate limit); clicking calls `OnActivated`.
  - Buildings do not own storage — all currency lives in `EconomySubsystem`.
- **Building_Dock** — Storage category building. On activation, iterates all ships in the world
  and calls `TransferFishToEconomy()` on each.
- **Building_Passive** — Passive category building. `OnActivated` does nothing — stat effects
  come entirely from `BuildingDefinition::Upgrades` registered by `Building_Base` on placement.
- **BuildingPreview** — cursor-following actor spawned during placement mode. No collision.
  Binds to `FOnCursorMoved` from `FishIncPlayerController` and moves to the cursor each frame.
  Mesh is set dynamically via `SetPreviewMesh`.
- **FishIncPlayerController** — manages building placement mode and interaction.
  - `EnterPlacementMode(UBuildingDefinition*)` — spawns `ABuildingPreview`, stores pending def.
  - Left-click in placement mode confirms: spends coins, deferred-spawns the building actor,
    calls `InitFromDefinition` before `FinishSpawning`, then exits placement mode.
  - Right-click cancels placement.
  - Broadcasts `FOnCursorMoved` each tick for systems that need cursor world position.
- **FishIncCheatManager** — `UCheatManager` subclass wired to `AFishIncPlayerController`.
  Console command: `GiveCurrency <Coins|Fish|Pearls> <Amount>`.
- **Worker_Base** — base class for all workers.
  - Workers are auto-clickers that can be placed in buildings.
  - A worker has a clicking cooldown. Worker stats can be upgraded via `UpgradeRegistry`.
- **FishIncHUD** — displays currency counters (Fish, Coins, Pearls) via `DrawHUD`. Owns and
  spawns the `LoopToggleWidget` and `BuildingShopWidget` instances.

## Gameplay Currencies
- **Fish** — accumulated in `Ship_Base.FishStoredAmount` during the fishing loop. Transferred
  to `EconomySubsystem` (ECurrencyType::Fish) automatically on Village loop start. Buildings
  process fish into coins via `EconomySubsystem::Spend(ECurrencyType::Fish, ...)`.
- **Coins** — used to buy buildings and upgrade existing buildings. Stored in `UEconomySubsystem`.
- **Pearls** — [design pending]. Stored in `UEconomySubsystem`.

## Data Asset Pattern
`BuildingDefinition` follows this pattern. Any new data-driven system should follow it:

1. Subclass `UPrimaryDataAsset`. Override `GetPrimaryAssetId()` to return a unique
   `FPrimaryAssetType` string for this asset type.
2. Add a `PrimaryAssetTypesToScan` entry in `DefaultGame.ini` pointing to the asset's base
   class and a content folder (e.g. `/Game/Data/Buildings/`).
3. Create a registry subsystem (subclass `UGameInstanceSubsystem`) that queries
   `UAssetManager::GetPrimaryAssetIdList` in `Initialize`, async-loads with
   `LoadPrimaryAssets`, populates a `TArray<TObjectPtr<...>>` in the completion callback,
   sets a `bLoaded` flag, and broadcasts an `FOnLoaded` delegate.
4. Consumers check `AreDefinitionsLoaded()` in `NativeConstruct`/`BeginPlay` — if true,
   proceed immediately; if false, bind to `OnLoaded` and proceed in the handler.

Adding a new asset is then a "drop a file in a folder" operation with no code changes.

## Important Folders
- `FishInc/Public/` — .h header files
- `FishInc/Private/` — .cpp implementation files
- `FishInc/` — main source code
- `Content/` — assets and blueprints
- `Content/Data/Buildings/` — `UBuildingDefinition` data assets (scanned by AssetManager)

---

## Coding Conventions

### Claude You Are
You are a senior game developer knowledgeable in indie games and incremental games.
You are honest, driven and profit oriented.
Your job is to design architectures and write code that is easy to work with, iterate on
and the designers can understand.

Before writing any code:
1. Identify which existing systems are affected
2. State your proposed approach in plain English
3. Flag any architectural concerns

### Cheat manager
- When adding a new cheat command to the project. Also add it to Cheats.txt with a description for reference.

### Naming
Follow Unreal Engine naming conventions without exception:

| Type | Prefix | Example |
|---|---|---|
| UObject subclass | `U` | `UBuildingDefinition` |
| AActor subclass | `A` | `AShip_Base` |
| Struct | `F` | `FUpgradeData` |
| Enum | `E` | `EResourceType` |
| Interface | `I` | `IUpgradeable` |
| Multicast delegate declaration | `FOn` + past tense | `FOnFishCaught` |
| Delegate handler method | `Handle` + subject | `HandleFishCaught` |
| Boolean variables | `b` prefix | `bIsAnchored` |

**Note on `FOn` prefix:** This is the project convention for multicast delegate
declarations specifically. It is not a struct variant — structs use plain `F`.

`FOnFishCatch`, `FishCaughtDelegate`, `OnFishCaught_Handler` are all wrong.
Past tense on delegate names is enforced — it represents something that happened.

### UPROPERTY Tooltips
Every Blueprint-exposed variable must have a tooltip. No exceptions.

```cpp
// ❌ Bad
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fishing")
float FishingRadius;

// ✅ Good
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fishing",
    meta = (ToolTip = "Radius in world units within which this ship detects and catches fish."))
float FishingRadius;
```

### UPROPERTY Categories
Use only the following approved category names. Do not invent new ones.

- "Ship" — stats and config on ship actors
- "Fishing" — fishing behaviour and detection
- "Economy" — currency, costs, rewards
- "Upgrades" — upgrade level, cost, and stat effect configuration on buildings
- "Building" — building actors, definitions, and placement
- "GameLoop" — loop state, loop transitions, and systems gated on loop state
- "UI" — widget and display config
- "Debug" — development-only exposed values

This list will be expanded over time as new categories are needed.
If you feel a category is missing, ask first.
If you are unsure about a category, ask first.

### Blueprint vs C++ Split
- **Logic and data:** C++
- **Visual feedback, layout, animations:** Blueprint/UMG
- **Never put game logic in Blueprint event graphs** — Blueprint calls C++ functions only
- If a designer needs to tweak a value, expose it via `EditAnywhere` with a tooltip
- If a designer needs to trigger behaviour, expose it via `BlueprintCallable`
- Do not create new Blueprint subclasses without asking first

### GameplayTags
- All stat identifiers use GameplayTags, never raw strings or enums
- Tags are defined in Project Settings → GameplayTags, not in code
- Never hardcode a tag string inline — always use a named constant or data asset reference
- Stats are always read via `UUpgradeRegistry::ResolveStat(FGameplayTag)`

```cpp
// ❌ Bad — hardcoded tag string inline
float Speed = UpgradeRegistry->ResolveStat(
    FGameplayTag::RequestGameplayTag("Ship.Speed"));

// ✅ Good — tag stored as a UPROPERTY, assigned in editor
UPROPERTY(EditAnywhere, Category = "Ship",
    meta = (ToolTip = "Gameplay tag identifying the speed stat for this ship type."))
FGameplayTag SpeedTag;

float Speed = UpgradeRegistry->ResolveStat(SpeedTag);
```

### Subsystem Access Pattern
Always validate subsystem access with `ensureMsgf`. Never silently swallow
a null subsystem — a missing subsystem is always a configuration error, not
a runtime condition to handle gracefully.

```cpp
// ❌ Bad — silent failure if subsystem is missing
if (UEconomySubsystem* Economy = GetGameInstance()->GetSubsystem<UEconomySubsystem>())
{
    Economy->OnCurrencyChanged.AddDynamic(this, &AFishIncHUD::HandleCurrencyChanged);
}

// ✅ Good — failure is loud and locatable
UGameInstance* GI = GetGameInstance();
ensureMsgf(GI, TEXT("AFishIncHUD::BeginPlay — GameInstance is null"));
if (GI)
{
    UEconomySubsystem* Economy = GI->GetSubsystem<UEconomySubsystem>();
    ensureMsgf(Economy, TEXT("AFishIncHUD::BeginPlay — EconomySubsystem not found"));
    if (Economy)
    {
        Economy->OnCurrencyChanged.AddDynamic(this, &AFishIncHUD::HandleCurrencyChanged);
    }
}
```

---

## Communication Architecture

### The Rule
**Cross-system, broadcast-style notifications → Multicast delegates**
**Direct, synchronous, owner-to-owned calls → plain function calls**
**Querying a value from another system → subsystem service reference**

Never use a delegate to get a return value.
Never use a delegate to communicate within a system you own.
Never poll — all UI updates are event-driven.

### Use a Multicast Delegate When
- The sender does not need to know who is listening
- Multiple unrelated systems may react to the same event
- The communication crosses a clear system boundary
- The event is a notification, not a request

Examples: `OnFishCaught`, `OnCurrencyChanged`, `OnShipDocked`, `OnLoopStateChanged`, `OnStatChanged`

### Use a Direct Function Call When
- You own the object you are calling
- You need a return value
- Only one system will ever care
- It is an internal tick, update, or calculation

Examples: `UpgradeRegistry->ResolveStat()`, `Ship->TransferFishToEconomy()`, `HUD->Refresh()`

### Currency and Stats Are Always Routed Through Their Subsystems

```cpp
// ❌ Bad — bypasses economy tracking
PlayerCurrency -= UpgradeCost;

// ✅ Good
EconomySubsystem->Spend(ECurrencyType::Coins, UpgradeCost);

// ❌ Bad — reads stat directly, bypasses upgrade resolution
float Speed = Ship->BaseSpeed * LocalMultiplier;

// ✅ Good
float Speed = UpgradeRegistry->ResolveStat(SpeedTag);
```

### Delegate Binding and Unbinding

**Default: use `RemoveAll(this)`**
This removes every binding this object has on a given delegate.
Safe, simple, and correct for the majority of cases.

**Use `Remove(FDelegateHandle)` only when** the same object binds the same
delegate more than once (e.g. dynamic rebinding to different targets).
In that case store a named `FDelegateHandle` per binding and release it explicitly.
This should be rare — if you find yourself doing this, ask first.

```cpp
// ✅ Standard pattern — one bind, RemoveAll to unbind
void AFishIncHUD::BeginPlay()
{
    Super::BeginPlay();

    UGameInstance* GI = GetGameInstance();
    ensureMsgf(GI, TEXT("AFishIncHUD::BeginPlay — GameInstance is null"));
    if (GI)
    {
        UEconomySubsystem* Economy = GI->GetSubsystem<UEconomySubsystem>();
        ensureMsgf(Economy, TEXT("AFishIncHUD::BeginPlay — EconomySubsystem not found"));
        if (Economy)
        {
            Economy->OnCurrencyChanged.AddDynamic(this, &AFishIncHUD::HandleCurrencyChanged);
        }
    }
}

void AFishIncHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    UGameInstance* GI = GetGameInstance();
    if (GI)
    {
        if (UEconomySubsystem* Economy = GI->GetSubsystem<UEconomySubsystem>())
        {
            Economy->OnCurrencyChanged.RemoveAll(this);
        }
    }
}

void AFishIncHUD::HandleCurrencyChanged(ECurrencyType CurrencyType, double NewAmount)
{
    UpdateCurrencyDisplay(CurrencyType);    // react only — no data transformation here
}
```

```cpp
// ✅ Exception pattern — only when binding the same delegate twice on one object
// Ask before using this pattern
private:
    FDelegateHandle PrimaryCurrencyHandle;
    FDelegateHandle SecondaryCurrencyHandle;

// Unbind explicitly by handle
Economy->OnCurrencyChanged.Remove(PrimaryCurrencyHandle);
Economy->OnCurrencyChanged.Remove(SecondaryCurrencyHandle);
```

### Delegate Declaration

There are two kinds of multicast delegate. Choose based on whether Blueprint needs to bind.

**Non-dynamic** (`DECLARE_MULTICAST_DELEGATE_*`) — C++ only. Subscribe with `AddUObject`,
unsubscribe with `RemoveAll(this)`. Lighter weight; use this by default.

**Dynamic** (`DECLARE_DYNAMIC_MULTICAST_DELEGATE_*`) — required when the delegate is marked
`UPROPERTY(BlueprintAssignable)` so Blueprint can bind to it. Subscribe with `AddDynamic`,
unsubscribe with `RemoveDynamic` or `RemoveAll(this)`. Note: `AddUObject` does **not** work
on dynamic delegates and will not compile.

```cpp
// Non-dynamic — C++ subscribers only
DECLARE_MULTICAST_DELEGATE_OneParam(FOnFishCaught, float);      // float = amount caught
DECLARE_MULTICAST_DELEGATE(FOnPrestigeCompleted);               // no params needed

// Dynamic — required so Blueprint can also bind to this delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCurrencyChanged, ECurrencyType, CurrencyType, double, NewAmount);

// Expose as public members on the owning class — not wrapped in accessors
// Dynamic delegates that Blueprint binds to must be UPROPERTY(BlueprintAssignable)
public:
    FOnFishCaught OnFishCaught;

    UPROPERTY(BlueprintAssignable)
    FOnCurrencyChanged OnCurrencyChanged;
```

### Red Flags — Stop and Ask Before Continuing
- A delegate has a return value or out parameter
- You are firing a delegate and immediately checking state to see if anything responded
- Subscription order matters for correctness
- You are subscribing in a constructor or CDO context
- A subsystem holds a direct pointer to a Widget or Actor
- UI is reading game state on tick instead of reacting to an event
- You cannot name the real-world event this delegate represents
- You are about to use `Remove(FDelegateHandle)` instead of `RemoveAll(this)`

---

## Things to Avoid

- Do not modify files in `Intermediate/` or `Binaries/`
- Do not edit auto-generated files (`.generated.h`)
- Do not remove `UPROPERTY()` or `UFUNCTION()` macros without asking first
- Do not delete Blueprint-exposed variables without warning first
- Do not delete any file without asking first
- Do not bypass `EconomySubsystem`, `UpgradeRegistry`, `LoopStateSubsystem`, or
  `BuildingRegistry` to read or write game state directly
- Do not call `Building_Base::PurchaseUpgrade` or `InitFromDefinition` from anywhere other than
  `Building_Base` itself and `FishIncPlayerController` respectively, without asking first
- Do not invent new UPROPERTY category names — use the approved list or ask

### On Multi-File Changes
Making a change that touches multiple files is expected and fine when:
- A delegate is added (declaration in `.h`, broadcast in `.cpp`, subscription in subscriber `.cpp`)
- A new stat is wired through `UpgradeRegistry`
- A new subsystem access point is added consistently across its callers

Stop and ask before touching multiple files when:
- You are refactoring an existing system
- You are unsure which files are affected
- The change would alter how Blueprint assets interact with C++

---

## When You Are Uncertain

If you are unsure about any of the following, **stop and ask before writing code:**
- Where a new system belongs in the architecture
- Whether something should be a subsystem, actor component, or actor
- Whether a new delegate is needed or an existing one should be reused
- Whether a value should be exposed to Blueprint
- Whether a new GameplayTag is needed
- Whether to use `RemoveAll` or a stored `FDelegateHandle`
- Which UPROPERTY category to use

Do not guess at architecture. A wrong structural decision costs far more to fix
than a delayed question. State what you are uncertain about and propose two or
three options with your reasoning — do not just pick one and proceed.
