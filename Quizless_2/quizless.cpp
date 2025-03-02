#include "TOK.h"

#include <iostream>
#include <fstream>
#include <windows.h>

char* CLS = (char*)"\033[H\033[J";
char* ccolor = (char*)"\033[0m";
char* scolor = (char*)"\033[33m";

char* qmustend = (char*)".qz";
char* lmustend = (char*)".qzl";

void show_console_cursor(bool showFlag) {
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO     cursorInfo;

	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = showFlag;
	SetConsoleCursorInfo(out, &cursorInfo);
}

void concat_dump(int count, char** strings, int selected) {
	if (count <= 0 || !strings) return;

	size_t total_length = 0;
	for (int i = 0; i < count; ++i) {
		total_length += strlen(strings[i]);
		total_length += 1;
		total_length += strlen(ccolor);
	}

	char* result = (char*)malloc(total_length + 1);
	if (!result) return;

	char* current_position = result;

	size_t clen = strlen(CLS);
	memcpy(current_position, CLS, clen);
	current_position += clen;

	for (int i = 0; i < count; ++i) {
		if (i == selected) {
			size_t scolor_len = strlen(scolor);
			memcpy(current_position, scolor, scolor_len);
			current_position += scolor_len;
		}

		size_t len = strlen(strings[i]);
		memcpy(current_position, strings[i], len);
		current_position += len;

		if (i == selected) {
			size_t ccolor_len = strlen(ccolor);
			memcpy(current_position, ccolor, ccolor_len);
			current_position += ccolor_len;
		}

		*current_position = '\n';
		current_position++;
	}

	*current_position = '\0';

	printf(result);

	return;
}

#define QPERFILE 50
#define QUIZMAX 100
#define NAMEMAX 30
#define TEXTMAX 100
#define CHOICES 5

typedef struct qzl {
	int nquiz;
	char quiznames[QUIZMAX][NAMEMAX];
	char authors[QUIZMAX][NAMEMAX];
	int quizids[QUIZMAX];
} qzl;

typedef struct qz {
	char arquestions[QPERFILE][TEXTMAX]; // covered
	char choice[QPERFILE][CHOICES][TEXTMAX]; // Covered for the most part
	int answer[QPERFILE]; // 0 < answer <= choices
	int qtype[QPERFILE]; // covered
	int nquestions = 0;	// Used as incrementor

	int quizid;	// Some generator required
	char title[40];	// covered
	char author[30]; // covered
} qz;

typedef struct menu {
	char* menus[20];
	void (*function)(int);
	int nmenu;
} menu;

typedef struct qzresult {
	int quizid;
	int score;
	int maxscore;
	int signature;
} qzresult;

void qmenu_action(int);
void qtmenu_action(int);
void tfmenu_action(int);
void cqmenu_action(int);
void mmenu_action(int);
void startup(void);

qz quiz;

void static menus(menu insert, bool esc_action, bool enter_action) {
	int selected = 0;

	for (int i = 0; i < 4096; i++) {
		concat_dump(insert.nmenu, insert.menus, selected);

		Sleep(150);
		
		int input = 0;
		// Input
		while (!input) {
			if (GetKeyState(VK_ESCAPE) & 0x8000 or GetKeyState(0x51) & 0x8000) {
				if (esc_action) { exit(0); }
				return;
			}
			if ((GetKeyState(VK_RETURN) & 0x8000) != 0) {
				Sleep(50);
				insert.function(selected);
				if (enter_action) {
					return;
				}
				input++;
			}

			if (GetAsyncKeyState(VK_DOWN) or GetAsyncKeyState(0x53)) {
				selected = ((insert.nmenu - 1) < (selected + 1)) ? (insert.nmenu - 1) : (selected + 1);
				input++;
			}
			if (GetAsyncKeyState(VK_UP) or GetAsyncKeyState(0x57)) {
				selected = ((0) > (selected - 1)) ? (0) : (selected - 1);
				input++;
			}
		}
	}
}

void qmenu_action(int selected) {
	return;
}

void qtmenu_action(int selected) {
	quiz.qtype[quiz.nquestions] = selected;
}

void tfmenu_action(int selected) {
	quiz.answer[quiz.nquestions] = selected ? 0 : 1;
}

void cqmenu_action(int selected) {
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
	show_console_cursor(true);
	
	switch (selected) {
	case 0:
	{
		char quizname[40];
		do {
			printf("%sWhat do you want your quiz to be named?\n ", CLS);

			fgets(quizname, sizeof(quizname), stdin);

			size_t len = strlen(quizname);
			if (len > 0 && quizname[len - 1] == '\n') {
				quizname[len - 1] = '\0';
			}
		} while (strlen(quizname) == 0);
		
		show_console_cursor(false);

		printf("%sThe quiz will be called:\n%s", CLS, quizname);

		memcpy(quiz.title, quizname, sizeof(quizname));
		
		Sleep(3000);

		break;
	}
	case 1:
	{
		char authorname[30];
		do {
			printf("%sWhat do you want your author name to be?\n ", CLS);

			fgets(authorname, sizeof(authorname), stdin);

			size_t len = strlen(authorname);
			if (len > 0 && authorname[len - 1] == '\n') {
				authorname[len - 1] = '\0';
			}
		} while (strlen(authorname) == 0);
		
		show_console_cursor(false);

		printf("%sYou will be called:\n%s", CLS, authorname);

		memcpy(quiz.author, authorname, sizeof(authorname));
		
		Sleep(2500);

		break;
	}
	case 2:
	{
		menu QT;
		QT.nmenu = 3;
		QT.menus[0] = (char*)"Comparison";
		QT.menus[1] = (char*)"Multiple Choice";
		QT.menus[2] = (char*)"True/False";
		QT.function = qtmenu_action;

		char question[50];
		do {
			printf("%sWhat is the Question?\n ", CLS);

			fgets(question, sizeof(question), stdin);

			size_t len = strlen(question);
			if (len > 0 && question[len - 1] == '\n') {
				question[len - 1] = '\0';
			}
		} while (strlen(question) == 0);

		memcpy(quiz.arquestions[quiz.nquestions], question, sizeof(char) * 50);
		
		show_console_cursor(false);
		menus(QT, false, true);
		show_console_cursor(true);
	
		switch (quiz.qtype[quiz.nquestions]) {
		case 0:
			char answer[TEXTMAX];
			do {
				printf("%sWhat is the answer to your question?\n ", CLS);
					
				fgets(answer, sizeof(answer), stdin);

				size_t len = strlen(answer);
				if (len > 0 && answer[len - 1] == '\n') {
					answer[len - 1] = '\0';
				}
			} while (strlen(answer) == 0);

			memcpy(quiz.choice[quiz.nquestions][0], answer, sizeof(char) * TEXTMAX);

			break;
		case 1:
			break;
		case 2:
			menu TF;
			TF.nmenu = 2;
			TF.menus[0] = (char*)"TRUE";
			TF.menus[1] = (char*)"FALSE";
			TF.function = tfmenu_action;

			quiz.answer[quiz.nquestions] = 0;
			menus(TF, false, true);

			quiz.nquestions += 1;

			break;
		}

		Sleep(150);

		break;
	}
	case 3:
	{
		break;
	}
	case 4:
	{
		break;
	}
	case 5:
	{
		break;
	}
	}

	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));

	return;
}

void mmenu_action(int selected) {
	menu CQM;
	CQM.nmenu = 6;
	CQM.menus[0] = (char*)  "Quiz Title";
	CQM.menus[1] = (char*)  "Quiz Author";
	CQM.menus[2] = (char*)  "Add Question";
	CQM.menus[3] = (char*) "View Questions";
	CQM.menus[4] = (char*)  "Reorder Questions";
	CQM.menus[5] = (char*)  "Save and Upload Quiz";
	CQM.function = cqmenu_action;

	switch (selected) {
	case 0:
		qmenu_action(selected);
		break;
	case 1:
		menus(CQM, false, false);
		break;
	case 2:
		system("start help.txt");
		break;
	case 3:
		exit(0);
	}

	return;
}

void startup(void) {
	menu MM;
	MM.nmenu = 4;
	MM.menus[0] = (char*) "Start Quizless";
	MM.menus[1] = (char*)  "Create a Quiz";
	MM.menus[2] = (char*)  "Help";
	MM.menus[3] = (char*)  "Exit";
	MM.function = mmenu_action;
	printf("%sWelcome to Quizless 2! Start by choosing what to do:\n", CLS);

	menus(MM, true, false);
}

int main(void) {
	show_console_cursor(false);

	// Starting
	startup();

	return 0;
}
