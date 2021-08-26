#ifndef _OFFSET_HPP
#define _OFFSET_HPP

// includes for this header
#include <cstdint>

// class for interacting with offsets
class Offsets {
public:
	/*
	 * Game Object Manager ( GOM ) 0x4D500
	 * UnityPlayer.dll + Offset
	 */
	uint64_t gameObjectManager = 0x17C1F18;

	/**
	 * protected string _displayName
	 * BasePlayer + Offset
	 */
	uint64_t playerDisplayName = 0x698;

	/**
	 * public ulong userID
	 * BasePlayer + Offset
	 */
	uint64_t playerUserID = 0x20;

	/**
	 * public PlayerModel playerModel
	 * BasePlayer + Offset
	 */
	uint64_t playerModel = 0x4B0;

	/**
	 * internal bool visible 
	 * PlayerModel + Offset
	 */
	uint64_t visible = 0x248;

	/**
	 * public ModelState modelState
	 * BaseProjectile + Offset
	 */
	uint64_t modelState = 0x5C8;

	/**
	 * public float waterLevel;
	 * ModelState + Offset
	 */
	uint64_t waterLevel = 0x14;

	/**
	 * public int flags
	 * ModelState + Offset
	 */
	uint64_t modelFlags = 0x24;

	/**
	 * public Model model
	 * BaseEntity + Offset
	 */
	uint64_t entityModel = 0x120;

	/**
	 * internal bool isLocalPlayer
	 * PlayerModel + Offset
	 */
	uint64_t isLocalPlayer = 0x259;

	/**
	 * public Transform[] boneTransforms
	 * Model + Offset
	 */
	uint64_t boneTransforms = 0x48;

	/**
	 * public bool Frozen
	 * BasePlayer + Offset
	 */
	uint64_t playerFrozen = 0x4B8;

	/**
	 * private Vector3 newVelocity
	 * PlayerModel + Offset
	 */
	uint64_t playerVelocity = 0x1FC;

	/**
	 * interal Vector3 position
	 * PlayerModel + Offset
	 */
	uint64_t playerPosition = 0x1D8;
	

	/**
	 * public float maxDistance
	 * BaseMelee + Offset
	 */
	uint64_t maxDistance = 0x280;

	/**
	 * public bool blockSprintOnAttack
	 * BaseMelee + Offset
	 */
	uint64_t blockSprintOnAttack = 0x289;

	/**
	 * public bool adminCheat
	 * BaseMovement + Offset
	 */
	uint64_t adminCheat = 0x18;

	/**
	 * public BaseMovement movement
	 * BasePlayer + Offset
	 */
	uint64_t playerBaseMovement = 0x4D8;

	/**
	 * public float clothingMoveSpeedReduction
	 * BasePlayer + Offset
	 */
	uint64_t clothingMoveSpeedReduction = 0x6FC;

	/**
	 * public bool clothingBlocksAiming
	 * BasePlayer + Offset
	 */
	uint64_t clothingBlocksAiming = 0x6F8;

	/**
	 * public bool equippingBlocked
	 * BasePlayer + Offset
	 */
	uint64_t equippingBlocked = 0x708;

	/**
	 * public float clothingWaterSpeedBonus
	 * BasePlayer + Offset
	 */
	uint64_t clothingWaterSpeedBonus = 0x700;

	/**
	 * public float timeScale;
	 * LTSeq + Offset
	 */
	uint64_t timeScale = 0x2C;

	/**
	 * private float groundAngle
	 * PlayerWalkMovement + Offset
	 */
	uint64_t groundAngle = 0xB4;

	/**
	 * private float groundAngleNew
	 * PlayerWalkMovement + Offset
	 */
	uint64_t groundAngleNew = 0xB8;

	/**
	 * public float maxAngleClimbing;
	 * PlayerWalkMovement + Offset
	 */
	uint64_t maxAngleClimbing = 0x80;

	/**
	 * public float maxAngleWalking;
	 * PlayerWalkMovement + Offset
	 */
	uint64_t maxAngleWalking = 0x7C;

	/**
	 * public float groundTime;
	 * PlayerWalkMovement + Offset
	 */
	uint64_t groundTime = 0xBC;

	/**
	 * public PlayerEyes eyes;
	 * BasePlayer + offset
	 */
	uint64_t playerEyes = 0x640;

	/**
	 * private Vector3 viewOffset
	 * PlayerEyes + offset
	 */
	uint64_t ViewOffset = 0x38;

	/**
	 * private Quaternion <bodyRotation>k__BackingField;
	 * PlayerEyes + offset
	 */
	uint64_t bodyRotation = 0x44;

	/**
	 * public static readonly Vector3 EyeOffset;
	 * PlayerEyes + offset
	 */
	uint64_t EyeOffset = 0x0;

	/**
	 * private float currentHoldProgress;
	 * CompoundBowWeapon + Offset
	 */
	uint64_t currentHoldProgress = 0x3B0;

	/**
	 * public float movementPenaltyRampUpTime;
	 * CompoundBowWeapon + Offset
	 */
	uint64_t movementPenaltyRampUpTime = 0x378;

	/**
	 * public float stringHoldDurationMax;
	 * CompoundBowWeapon + Offset
	 */
	uint64_t stringHoldDurationMax = 0x368;

	/**
	 * public PlayerInput input
	 * BasePlayer + Offset
	 */
	uint64_t playerInput = 0x4D0;

	/**
	 * public float clientTickInterval
	 * BasePlayer + Offset
	 */
	uint64_t clientTickInterval = 0x608;

	/**
	 * public InputState state;
	 * Input + Offset
	 */
	uint64_t state = 0x20;

	/**
	 * public InputMessage current;
	 * state + Offset
	 */
	uint64_t current = 0x10;
	
	/**
	 * camera manager + Offsets
	 */
	uint64_t camera_fov = 0x18;

	uint64_t camera_manager = 0x18;

	/**
	 * public Vector3 aimAngles;
	 * current + Offset
	 */
	uint64_t aimAngles = 0x18;

	/**
	 * public Vector3 recoilAngles
	 * PlayerInput + Offset
	 */
	uint64_t recoilAngles = 0x64;

	/**
	 * private Vector3 bodyAngles
	 * PlayerInput + Offset
	 */
	uint64_t bodyAngles = 0x3C;

	/**
	 * private float jumpTime
	 * PlayerWalkMovement + Offset
	 */
	uint64_t jumpTime = 0xC0;

	/**
	 * private float landTime
	 * PlayerWalkMovement + Offset
	 */
	uint64_t landTime = 0xC4;

	/**
	 * private float gravityMultiplier
	 * PlayerWalkMovement + Offset
	 */
	uint64_t gravityMultiplier = 0x74;

	/**
	 * private float gravityMultiplierSwimming
	 * PlayerWalkMovement + Offset
	 */
	uint64_t gravityMultiplierSwimming = 0x78;

	/**
	 * private float _health
	 * BaseCombatEntity + Offset
	 */
	uint64_t health = 0x214;

	/**
	 * private EntityRef mounted
	 * BasePlayer + Offset
	 */
	uint64_t mountedEntRef = 0x5D0;

	/**
	 * private uint clActiveItem
	 * BasePlayer + Offset
	 */
	uint64_t clActiveItem = 0x5B0;

	/**
	 * public PlayerInventory inventory
	 * BasePlayer + Offset
	 */
	uint64_t playerInventory = 0x648;

	/**
	 * public ItemContainer containerBelt
	 * PlayerInventory + Offset
	 */
	uint64_t containerBelt = 0x28;

	/**
	 * public List<Item> contents
	 * ItemContainer + Offset
	 */
	uint64_t beltContentsList = 0x38;

	/**
	 * public BasePlayer.PlayerFlags playerFlags
	 * BasePlayer + Offset
	 */
	uint64_t playerFlags = 0x638;

	/**
	 * public float recoilYawMin
	 * RecoilProperties + Offset
	 */
	uint64_t recoilYawMin = 0x18;

	/**
	 * public float recoilYawMax
	 * RecoilProperties + Offset
	 */
	uint64_t recoilYawMax = 0x1C;

	/**
	 * public float recoilPitchMin
	 * RecoilProperties + Offset
	 */
	uint64_t recoilPitchMin = 0x20;

	/**
	 * public float recoilPitchMax
	 * RecoilProperties + Offset
	 */
	uint64_t recoilPitchMax = 0x24;

	/**
	 * protected bool attackReady
	 * BowWeapon + Offset
	 */
	uint64_t attackReady = 0x350;

	/**
	 * public float successFraction
	 * FlintStrikeWeapon + Offset
	 */
	uint64_t successFraction = 0x350;

	/**
	 * public bool aiming
	 * BaseProjectile + Offset
	 */
	uint64_t aiming = 0x2F9;

	/**
	 * private List<Projectile> createdProjectiles
	 * BaseProjectile + Offset
	 */
	uint64_t createdProjectiles = 0x348;

	/**
	 * public float thickness
	 * Projectile + Offset
	 */
	uint64_t projectileThickness = 0x2C;

	/**
	 * public ItemModProjectile mod
	 * Projectile + Offset
	 */
	uint64_t projectileItemMod = 0xE8;

	/**
	 * public float projectileSpread
	 * ItemModProjectile + Offset
	 */
	uint64_t projectileSpread = 0x30;

	/**
	 * public float timeToTakeMin
	 * RecoilProperties + Offset
	 */
	uint64_t timeToTakeMin = 0x28;

	/**
	 * public float timeToTakeMax
	 * RecoilProperties + Offset
	 */
	uint64_t timeToTakeMax = 0x2C;

	/**
	 * public float movementPenalty
	 * BasePlayer + Offset
	 */
	uint64_t movementPenalty = 0x34;

	/**
	 * public float aimCone
	 * BaseProjectile + Offset
	 */
	uint64_t aimCone = 0x6B8;

	/**
	 * public float aimSway
	 * BaseProjectile + Offset
	 */
	uint64_t aimSway = 0x2C0;

	/**
	 * public bool automatic
	 * BaseProjectile + Offset
	 */
	uint64_t automatic = 0x278;

	/**
	 * public float hipAimCone
	 * BaseProjectile + Offset
	 */
	uint64_t hipAimCone = 0x2DC;

	/**
	 * public float aimConePenaltyMax
	 * BaseProjectile + Offset
	 */
	uint64_t aimConePenaltyMax = 0x2E4;

	/**
	 * public float aimconePenaltyPerShot
	 * BaseProjectile + Offset
	 */
	uint64_t aimconePenaltyPerShot = 0x2E0;

	/**
	 * public float aimconePenaltyRecoverTime
	 * BaseProjectile + Offset
	 */
	uint64_t aimconePenaltyRecoverTime = 0x2E8;

	/**
	 * public float stancePenaltyScale
	 * BaseProjectile + Offset
	 */
	uint64_t stancePenaltyScale = 0x2F0;

	/**
	 * private static float _fov;
	 * Graphics + Offset
	 */
	uint64_t _fov = 0x18;
};
extern Offsets* offsets;

#endif // _OFFSET_HPP