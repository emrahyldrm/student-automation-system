/* PR_111044056.h
 *
 * 
 */
 

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<time.h>

#define NM_SN_SIZE			50
#define ID_SIZE				20
#define PREQ_SIZE			20
#define TC_SIZE				12
#define PASS_SIZE			20
#define TYPE_SIZE			15
#define FILE_ERROR			212
#define USER_COURSE_ERR 	412
#define YES					1
#define NO					0
#define ONE					1
#define ALL					100
#define OPEN				999
#define CLOSE				998
#define AVAILABLE			888
#define NAVAILABLE			887
#define STUDENT				99
#define INSTRUCTOR			98
#define BEGIN				255
#define FINAL				256
#define GRADE				256
#define IF_OPEN				256
#define TRUE				1
#define FALSE				0

typedef struct
{
	char tc[TC_SIZE], name[NM_SN_SIZE], 
		 surname[NM_SN_SIZE], no[NM_SN_SIZE];
	int  startYear, courseCount;
} student_t;


typedef struct
{
	char tc[TC_SIZE], name[NM_SN_SIZE], surname[NM_SN_SIZE];
}instructors_t;


typedef struct
{
	char ID[ID_SIZE];
}preq_t;


typedef struct
{
	char ID[ID_SIZE], name[NM_SN_SIZE];
	char instructorName[NM_SN_SIZE], instructorSurname[NM_SN_SIZE], instructorID[ID_SIZE];
	preq_t preq[PREQ_SIZE];
	int isOpen, isAvailable, preqNum, stdCount;
}courses_t;


typedef struct
{
	char ID[TC_SIZE], type[TYPE_SIZE];
	char userName[NM_SN_SIZE], password[PASS_SIZE]; 
}users_t;

typedef struct
{
	char ID[TC_SIZE], type[TYPE_SIZE];
	char name[NM_SN_SIZE], surname[NM_SN_SIZE];
	char userName[NM_SN_SIZE], password[PASS_SIZE];

}curr_user_t;


typedef struct
{
	char name[NM_SN_SIZE], surname[NM_SN_SIZE];
	char ID[ID_SIZE];
	double grade;

}crs_std_t;

typedef struct
{
	char ID[ID_SIZE],  name[NM_SN_SIZE];
	double grade;
}std_crs_t;



extern int 	checkUser(const char *name, const char *password);

extern int 	fileControls(void);

extern int 	checkAllFiles();

extern void getAccessInfo(curr_user_t *currentUser);

extern int 	getCommand(int type);

extern int 	checkBinaryFiles();

extern int 	checkCsvFiles();

extern char *readStudents(student_t *student, FILE *studentFile);

extern void writeAllStudent();

extern void writeAllInstructors();

extern char *readInstructors(instructors_t *instructor, FILE *instructorFile);

extern char *readCourses(courses_t *course, FILE *courseFile);

extern void writeAllCourses();

extern char *readUsers(users_t *user, FILE *userFile);

extern void writeAllUsers();

extern int checkAvailableOpen(const char *courseID);

extern int preqControl(const char *studentID, const char *courseID);

extern int 	enrolCourse(const char *courseID, const char *studentID);

extern int 	listCourses4Add();

extern int listAndSaveEnrolledCourses(const char *studentID, int saveMode);

extern int 	disenrolCourse(const char *courseID, const char *studentID, int byWho);

extern int createTranscript(const char *userID);

extern int listCourses4Instructor(const char *instructorID, int forWhat, int saveMode);

extern int processCourse(const char *courseID, int stateFlag, const char *userID);

extern void saveOpenCourses(const char *instructorID, courses_t *courses, int size);

extern int listStudents4Grade(const char *courseID, const char *currentUserID, int saveMode);

extern double getGrade(void);

extern void saveGrades(courses_t *course, crs_std_t *students);

extern int assignGrade(const char *studentID, const char *courseID, const char *currentUserID, double grade);

extern int isGraded(const char *courseID, const char *studentID, const char *currentUserID, int forWhat);

extern void finalizeCourse(const char *courseID, const char *currentUserID);

extern void findCurrentUserInfo(curr_user_t *currentUser);

extern void printMenu4Students(const char *name, const char *surname);

extern void printMenu4Instructors(const char *name, const char *surname);

extern void getIDCommand(char *commandID);


/******************************************************************************/

extern int studentChoice1(curr_user_t currentUser);

extern int studentChoice2(curr_user_t currentUser);

extern int instructorChoice1(curr_user_t currentUser);

extern int instructorChoice2(curr_user_t currentUser);

extern int instructorChoice3(curr_user_t currentUser);

extern int instructorChoice4(curr_user_t currentUser);

extern int instructorChoice5(curr_user_t currentUser);

extern int instructorChoice6(curr_user_t currentUser);

extern int instructorChoice8(curr_user_t currentUser);