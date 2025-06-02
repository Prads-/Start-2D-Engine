#ifndef DATA_PACKETS_H
#define DATA_PACKETS_H

#include "declarations.h"

#define DATA_PACKET_BUFFER_SIZE					300
#define DATA_PACKET_STREAM_DATA_SIZE			288
#define DATA_PACKET_NUMBER_OF_STREAM_PACKET		((1024 * 768 * 3) / DATA_PACKET_STREAM_DATA_SIZE)

#define DPTYPE_CLIENT_HANDSHAKE					0

#define DPTYPE_SERVER_GAME_INFO					1
#define DPTYPE_CLIENT_GAME_INFO_ACK				2

#define DPTYPE_SERVER_GAME_START				3

#define DPTYPE_PLAYERS_NAME						4

#define DPTYPE_POSITIONS						5
#define DPTYPE_CLIENT_INPUT						6

#define DPTYPE_CLIENT_CANCEL_GAME				7
#define DPTYPE_SERVER_CANCEL_GAME				8	

#define DPTYPE_SERVER_VICTORY					9

#define DPTYPE_SERVER_START_BACKGROUND_STREAM	10
#define DPTYPE_SERVER_BACKGROUND_STREAM			11
#define DPTYPE_CLIENT_BACKGROUND_STREAM_ACK		12

#define CLIENT_INPUT_UP_KEY						1
#define CLIENT_INPUT_DOWN_KEY					2
#define CLIENT_INPUT_RIGHT_KEY					4
#define CLIENT_INPUT_LEFT_KEY					8
#define CLIENT_INPUT_LEFT_MOUSE					0x10
#define CLIENT_INPUT_MID_MOUSE					0x20
#define CLIENT_INPUT_RIGHT_MOUSE				0x40
#define CLIENT_INPUT_C_KEY						0x80

#define SERVER_PLAYER_IS_SHIELDED				1
#define SERVER_PLAYER_IS_DAMAGED				2
#define SERVER_PLAYER_HAS_BALL					4
#define SERVER_PLAYER_MY_SHIP					8
#define SERVER_PLAYER_IS_DEAD					0x10
#define SERVER_PLAYER_HAS_SHOT_STUN				0x20
#define SERVER_PLAYER_HAS_SHOT_BALL				0x40
#define SERVER_PLAYER_IS_STUNNED				0x80

#define SERVER_BALL_HIT_FLAG					1
#define SERVER_BALL_IS_CATCHABLE				2
#define SERVER_BALL_IS_TAKEN					4
#define SERVER_BALL_EXPLODE						8
#define SERVER_BALL_IS_INVISIBLE				0x10

#define SERVER_SPECIAL_COOLDOWN					0x80
#define SERVER_SECONDARY_COOLDOWN				0x40
#define SERVER_PLAYER_HEALTH					0x3F

#define SERVER_PICKUP_IS_ACTIVE					0x8000
#define SERVER_PICKUP_X							0x7FFF

#pragma pack (push)
#pragma pack (1)
	struct DataPacketDataType {
		uint16 dpType;
	};
	struct DataPacketClientHandShake {
		uint16 dpType;

		DataPacketClientHandShake() {
			dpType = DPTYPE_CLIENT_HANDSHAKE;
		}
	};
	struct DataPacketServerGameInfo {
		uint16 dpType;
		int32 mapId;
		uint8 numberOfPlayer, hasBots;
		uint8 serverName[20];

		DataPacketServerGameInfo() {
			dpType = DPTYPE_SERVER_GAME_INFO;
		}
	};
	struct DataPacketClientGameInfoAck {
		uint16 dpType;
		uint8 name[20];
		uint32 characterId;

		DataPacketClientGameInfoAck() {
			dpType = DPTYPE_CLIENT_GAME_INFO_ACK;
			for (int i = 0; i < 20; ++i) {
				name[i] = 0;
			}
		}
	};
	struct DataPacketServerGameStart {
		uint16 dpType;
		uint32 characterIds[MAXIMUM_NUMBER_OF_CLIENTS];

		DataPacketServerGameStart() {
			dpType = DPTYPE_SERVER_GAME_START;
			for (int i = 0; i < MAXIMUM_NUMBER_OF_CLIENTS; ++i) {
				characterIds[i] = 0;
			}
		}
	};
	struct DataPacketPositions {
		uint16 dpType;
		uint32 dpId;
		struct OpponentPositionData {
			uint16 x, y, rotateDegree;
			uint8 flags;

			OpponentPositionData() {
				flags = 0;
			}
		} playersPosition[MAXIMUM_NUMBER_OF_CLIENTS];
		struct BallPositionData {
			uint16 x, y;
			uint8 flags;
			
			BallPositionData() {
				flags = 0;
			}
		} ballPosition;
		struct MiscPosition {
			uint8 flags, pickup;
			uint16 vMouseX, vMouseY;

			MiscPosition() {
				flags = 0;
			}
		} miscPosition;
		struct PickupPosition {
			uint16 x, y;
		}pickupPosition;

		DataPacketPositions() {
			dpType = DPTYPE_POSITIONS;
		}
	};
	struct DataPacketClientInput {
		uint16 dpType;
		uint8 inputFlags;
		uint16 mouseX, mouseY;
		
		DataPacketClientInput() {
			dpType = DPTYPE_CLIENT_INPUT;
			inputFlags = 0;
		}
	};
	struct DataPacketCancelGame {
		uint16 dpType;

		DataPacketCancelGame(bool server) {
			dpType = (server) ? DPTYPE_SERVER_CANCEL_GAME : DPTYPE_CLIENT_CANCEL_GAME;
		}
	};
	struct DataPacketVictoryPacket {
		uint16 dpType;

		DataPacketVictoryPacket() {
			dpType = DPTYPE_SERVER_VICTORY;
		}
	};
	struct DataPacketBackgroundStreamStart {
		uint16 dpType;

		DataPacketBackgroundStreamStart() {
			dpType = DPTYPE_SERVER_START_BACKGROUND_STREAM;
		}
	};
	struct DataPacketBackgroundStream {
		uint16 dpType, packetId;
		uint8 backgroundData[DATA_PACKET_STREAM_DATA_SIZE];

		DataPacketBackgroundStream() {
			dpType = DPTYPE_SERVER_BACKGROUND_STREAM;
		}
	};
	struct DataPacketBackgroundStreamAck {
		uint16 dpType, packetId;

		DataPacketBackgroundStreamAck() {
			dpType = DPTYPE_CLIENT_BACKGROUND_STREAM_ACK;
		}
	};
#pragma pack (pop)

#endif