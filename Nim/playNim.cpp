// playNim.cpp
// This set of functions are used to actually play the game.
// Play starts with the function: playNim() which is defined below

#include "nim.h"
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <random>

using std::string;
using std::cout;
using std::endl;
using std::cin;

void initializeBoard(char board[MAX_NIM_BOARD_SIZE])
{
	int numPiles = rand() % 6 + 3;
	board[0] = numPiles;
	for (int i = 0; i < numPiles; ++i){

		int numRocks = rand() % 20 + 1;;

		char rocks[2];
		rocks[0] = numRocks / 10;
		rocks[1] = numRocks % 10;
		board[1 + 2 * i] = rocks[0];
		board[2 * (i + 1)] = rocks[1];
	}
}

bool validateMove(char board[MAX_NIM_BOARD_SIZE], char move[MAX_SEND_BUFFER])
{
	bool result = true;

	if (strlen(move) != MAX_NIM_MOVE_SIZE) 
	{
		result = false;
	}
	else 
	{
		int pile = move[0] - '0' - 1;

		std::string rockNumBuffer(1, move[1]);
		rockNumBuffer += move[2];
		int rocksToRemove = stoi(rockNumBuffer);
		
		string rockPileNumBuffer(1, board[1 + 2 * pile]);
		rockPileNumBuffer += board[2 * (pile + 1)];
		int currentRocksInPile = stoi(rockPileNumBuffer);

		if (rocksToRemove > currentRocksInPile || pile == 0) 
		{
			result = false;
		}	
	}
	return result;
}

// BE SURE TO VALIDATE BEFORE UPDATING!!!!!!!!!!!
void updateBoard(char board[MAX_NIM_BOARD_SIZE], char move[MAX_SEND_BUFFER]) 
{
	int pileToMove = move[0] - '0' -1;

	std::string rockNumBuffer(1, move[1]);
	rockNumBuffer += move[2];
	int rocksToRemove = stoi(rockNumBuffer);

	string rockPileNumBuffer(1, board[1 + 2 * pileToMove]);
	rockPileNumBuffer += board[2 * (pileToMove + 1)];
	int currentRocksInPile = stoi(rockPileNumBuffer);

	currentRocksInPile -= rocksToRemove;

	char newRockNum[2];
	_itoa_s(currentRocksInPile, newRockNum, 10);

	board[1 + 2 * pileToMove] = newRockNum[0];
	board[2 * (pileToMove + 1)] = newRockNum[1];
}


void displayBoard(char board[MAX_NIM_BOARD_SIZE])
{
	std::cout << std::endl;
	int numPiles = board[0] - '0';
	std::cout << std::endl << "----------------------------------------" << std::endl;
	for (int i = 0; i < numPiles; ++i)
	{
		std::cout << "Rock Pile #" << (i + 1) << " -> ";
		std::string rockNumBuffer(1, board[1 + 2 * i]);
		rockNumBuffer += board[2 * (i + 1)];
		int numRocks = std::stoi(rockNumBuffer);

		for (int j = 0; j < numRocks; ++j)
		{
			std::cout << "* ";
		}
		for (int j = 0; j < 20 - numRocks; ++j)
		{
			std::cout << "  ";
		}
		std::cout << "(" << numRocks << ")" << " <- Rock Pile #" << (i + 1) << std::endl << "----------------------------------------";
	}
	std::cout << std::endl;
}

bool check4Win(char board[MAX_NIM_BOARD_SIZE])
{
	bool winner = true;

	int numPiles = board[0] - '0';
	for (int i = 1; i < (numPiles * 2) + 1 && winner == true; ++i) 
	{
		if (board[i] != '0') 
		{
			winner = false;
		}
	}

	return winner;
}


void getLocalUserMove(char board[MAX_NIM_BOARD_SIZE], char newMove[MAX_SEND_BUFFER])
{
	char playerMove[MAX_SEND_BUFFER];

	//dislay options
	std::cout << "It's your move!" << endl << "Move: Send a move in the format mnn" << endl
		<< "Comment: Send a comment in format CSome comment" << endl
		<< "Forfeit: Send F" << endl;

	//check if valid input
	bool validMove = false;
	do {
		std::cout << "Your move? ";
		std::cin >> playerMove;
		switch (playerMove[0])
		{
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			if (validateMove(board, playerMove))
			{
				validMove = true;
			}
			else
			{
				cout << "Incorrect move format. Try again." << endl;
			}
			break;
		case 'F':
			validMove = true;
			break;
		case 'C':
			if (strlen(playerMove) > 80)
			{
				cout << "Your comment is too long. Send a comment of 80 characters or less." << endl;
			}
			else
			{
				validMove = true;
			}
			break;
		default:
			cout << "Invalid input. Try again." << endl;
		}
	} while (!validMove);

	newMove = playerMove;
}

int playNim(SOCKET s, std::string remoteIP, std::string remotePort, int localPlayer)
{
	// This function plays the game and returns the value: winner.  This value 
	// will be one of the following values: noWinner, xWinner, oWinner, TIE, ABORT
	int winner = 0;
	char board[MAX_NIM_BOARD_SIZE];
	int opponent;
	char move[MAX_SEND_BUFFER];
	bool myMove;
	char dummyRecvBuffer[MAX_RECV_BUFFER];
	char dummyRecvBuffer2[MAX_RECV_BUFFER];
	bool isComment = true;
	int status;

	if (localPlayer == CLIENT_PLAYER) {
		std::cout << "FIRST MOVE" << std::endl;
		opponent = SERVER_PLAYER;
		myMove = true;

		wait(s, 2, 0);
		UDP_recv(s, board, MAX_SEND_BUFFER, dummyRecvBuffer, dummyRecvBuffer2);
	}
	else {
		std::cout << "NEXT MOVE" << std::endl;
		opponent = CLIENT_PLAYER;
		myMove = false;

		initializeBoard(board);
		UDP_send(s, board, MAX_SEND_BUFFER, remoteIP.c_str(), remotePort.c_str());
	}
	displayBoard(board);

	while (winner == 0) {

		if (myMove) {
			// Get my move & display board

			//Loop sending comments until move is sent
			while (isComment) {
				getLocalUserMove(board, move);
				if (move[0] != 'C') {
					isComment = false;
				}
				if(move[0] != 'F'){
					UDP_send(s, move, MAX_SEND_BUFFER, remoteIP.c_str(), remotePort.c_str());
				}
			}
			isComment = true;
			if(move[0] == 'F'){
				winner = opponent;
				cout << "You have forfeited." << endl;
			}
			else {
				std::cout << "Board after your move:" << std::endl;
				updateBoard(board, move);
				displayBoard(board);
			}
		}

		// Recieving the move
		else {
			std::cout << "Waiting for your opponent's move..." << std::endl << std::endl;

			while (isComment) {
				status = wait(s, WAIT_TIME, 0);
				UDP_recv(s, move, MAX_RECV_BUFFER, dummyRecvBuffer, dummyRecvBuffer2);
				if (status == 0 || move[0] != 'C') {
					isComment = false;
				}
				else {
					cout << "Comment: " << move << endl;
				}
			}
			isComment = true;
			if (status > 0 && validateMove(board, move)) {
				updateBoard(board, move);
				displayBoard(board);
			}
			else {
				winner = ABORT;
			}
		}
		myMove = !myMove;   // Switch whose move it is

		if (winner == ABORT) {
			std::cout << timestamp() << " - No response from opponent or invalid move received. You WIN!" << std::endl;
		}
		else {
			winner = check4Win(board);
		}

		if (winner == localPlayer)
			std::cout << "You WIN!" << std::endl;
		else if (winner == opponent)
			std::cout << "I'm sorry.  You lost" << std::endl;
	}

	return winner;
}