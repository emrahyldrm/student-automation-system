#include "PR_111044056.h"

#define TAKE_NAME(ID)		scanf("%s", ID)


int main(int argc, char **argv)
{
	int command;
	int loginState=FALSE;
	char ID[NM_SN_SIZE], commandID[ID_SIZE];
	student_t student;
	curr_user_t currentUser;
	instructors_t instructor;

	
	
	if(fileControls() == FILE_ERROR)
		return FILE_ERROR;
	
	
	
	if(argc == 3)
	{	
		strcpy(currentUser.userName, argv[1]);
		strcpy(currentUser.password, argv[2]);
	}
	else
		{
			printf("WRONG TERMINAL ACCESS\n");
			printf("TRY AGAIN !!\n");
			getAccessInfo(&currentUser);
		}
		
	do
	{
		loginState = checkUser(currentUser.userName, currentUser.password);
		if(!loginState)
		{
			printf("Wrong PASSWORD or USERNAME\n");
			printf("TRY AGAIN !!\n");
			getAccessInfo(&currentUser);
		}
		else
		{	
			findCurrentUserInfo(&currentUser);
		}
	}while(!loginState);

	
	system("clear");
	
	while(1)
	{		
	if(!strcmp(currentUser.type, "student"))
	{		
		
		printMenu4Students(currentUser.name, currentUser.surname);
		
		command = getCommand(STUDENT);
		
		switch(command)
		{
			case 1:
					studentChoice1(currentUser);
					break;
					
			case 2:
					studentChoice2(currentUser);
					break;
					
			case 3:
					system("clear");
					printf("\nENROLLED COURSES\n\n");
					listAndSaveEnrolledCourses(currentUser.ID, YES);
					break;
					
			case 4:
					system("clear");
					if(createTranscript(currentUser.ID) == USER_COURSE_ERR)
						printf("\nYou do not have any old course..\n");
					else
						printf("\nThe transcript has been saved to %s_transcript.txt\n", currentUser.ID);
					break;
					
			case 5:
					printf("Leving the system..Have a nice day\n");
					return 0;
				
				
		}		
			printf("______________________________________________");
			printf("______________________________________________\n\n");
	}
	
	
	if(!strcmp(currentUser.type, "instructor"))
	{
		
		printMenu4Instructors(currentUser.name, currentUser.surname);
		
		command = getCommand(INSTRUCTOR);
		
		switch(command)
		{
			case 1:
					instructorChoice1(currentUser);
					break;		
					
					
			case 2:
					instructorChoice2(currentUser);
					break;
					
					
			case 3:
					instructorChoice3(currentUser);
					break;
			
			case 4:
					instructorChoice4(currentUser);
					break;
			
			case 5:
					instructorChoice5(currentUser);
					break;
					
			case 6:
					instructorChoice6(currentUser);
					break;
					
			case 7:
					system("clear");
					printf("#\tThe Courses that Yours\t\t#\n");
					printf("-----------------------------------------\n");
					listCourses4Instructor(currentUser.ID, IF_OPEN, YES);
					printf("\n");
					
					break;
			
			case 8:
					instructorChoice8(currentUser);
					break;
		
			case 9:
					printf("Leving the system..Have a nice day\n");
					return 0;
		}
		printf("______________________________________________");
		printf("______________________________________________\n\n");
	}
	}

	return 0;
}


