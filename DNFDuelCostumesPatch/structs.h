typedef int8_t s8;
typedef uint8_t u8;
typedef int16_t s16;
typedef uint16_t u16;
typedef int32_t s32;
typedef uint32_t u32;
typedef int64_t s64;
typedef uint64_t u64;
typedef float f32;
typedef double f64;

typedef struct
{
	char* Field00;
	long Field04;
	int Field0C;
} SampleStruct;

typedef enum SIDE_ID {
	SIDE_1P = 0,
	SIDE_BEGIN = 0,
	SIDE_2P = 1,
	SIDE_COMMON = 2,
	SIDE_ID_NUM = 2,
	SIDE_ID_NUM_WITH_COMMON = 3,
	SIDE_ID_INVALID = 4
} SIDE_ID; // Strive copypasta

typedef enum EMemberID {
	MemberID_01 = 0,
	MemberID_Begin = 0,
	MemberID_02 = 1,
	MemberID_03 = 2,
	MemberID_MAX = 3,
	MemberID_INVALID = 4
} EMemberID; // Strive copypasta

struct FString {
	wchar_t* Name;
	int min;
	int max;
};

struct IDConvertTable {
	int SourceID;
	int DestID;
};

typedef struct
{
	u8 Select;
	u8 Start;
	u8 DPadUp;
	u8 DPadRight;
	u8 DPadDown;
	u8 DPadLeft;
	u8 L1;
	u8 R1;
	u8 L2;
	u8 R2;
	u8 L3;
	u8 R3;
	u8 Circle;
	u8 Cross;
	u8 Square;
	u8 Triangle;
	u8 RightStickUp;
	u8 RightStickRight;
	u8 RightStickDown;
	u8 RightStickLeft;
	f32 LeftStickXPos;
	f32 LeftStickYPos;
	f32 RightStickXPos;
	f32 RightStickYPos;
	f32 LeftTriggerDepth;
	f32 RightTriggerDepth;
}UE4PlayerInput;

typedef struct FSpawnPlayerInfo
{
	SIDE_ID SideID;                     // 0x0
	EMemberID MemberID;                 // 0x4
	u64 Unknown;
	FString CharaID;                    // 0x8
	FString SelfTeamMemberCharaID;      // 0x18
	FString EnemyTeamMemberCharaID;     // 0x28
	FString BGLayerNames;               // 0x38
	u32 CostumeID;                      // 0x48
	u32 ColorID;                        // 0x4C
	u32 ScriptType;                     // 0x50
	u32 Field54;                        // 0x54
	FString VoiceLoc;                   // 0x58
	u8 bREV2ModelFlag;                  // 0x68
	u8 Field69;                         // 0x69
	u8 Field6A;                         // 0x6A
	u8 Field6B;                         // 0x6B
	u32 Field6C;                        // 0x6C
	IDConvertTable* CostumeIDConvTable; // 0x70
	IDConvertTable* ColorIDConvTable;   // 0x78
	u32 CostumeIDConvTableNum;          // 0x80
	u32 ColorIDConvTableNum;            // 0x84
} FSpawnPlayerInfo; // Strive copypasta, most of it is probably wrong but the important stuff isnt, lol

typedef struct
{
	u8 Unknown[0x3394];
	s32 PlayerID;
	u8 Unknown2[0x2CC];
	s32 CanPlayerSelect; // 0x3664
} CharSelectStruct;

typedef struct
{
	s32 field00;
	s32 field04;
	s32 field08;
	s32 field0c;
	s32 field10;
	s32 field14;
	s32 field18;
	s32 field1c;
	s32 field20;
	s32 field24;
	s32 field28;
	s32 field2c;
	s32 field30;
	s32 field34;
	s32 field38;
	s32 field3c;
	s32 field40;
	s32 field44;
	s32 field48;
	s32 field4c;
	s32 field50;
	s32 field54;
	s32 field58;
	s32 field5c;
	s32 field60;
	s32 field64;
	s32 field68;
	s32 field6c;
	s32 field70;
	s32 field74;
} PostCSS;