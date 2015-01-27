#include "PR_111044056.h"

#ifndef TRUE
#define TRUE			1
#endif
#ifndef FALSE
#define FALSE			0
#endif
#ifndef FILE_ERROR
#define FILE_ERROR		212
#endif

#ifndef ACCESS_ERROR
#define ACCESS_ERROR	313
#endif

#define PATH_SIZE		100
#define PASS			101

#define DEBUG_CONTERS


#define TAKE_COMMAND(command)		scanf("%d", &command)



/************************************************************************
 * 																		*
 * kullanıcı giris bilgilerini kontrol eden fonksiyon					*
 * bilgiler eslesiyorsa TRUE, yanlis giris varsa FALSE return eder		*
 * 																		*
 ***********************************************************************/
int checkUser(const char *name, const char *password)
{
	int login = FALSE;
	users_t user;
	FILE *userFile;
	
	
	userFile = fopen("Users.bin", "rb");
	
	while(!feof(userFile))
	{
		fread(&user, sizeof(users_t), 1, userFile);
		if(!strcmp(user.userName, name) && !strcmp(user.password, password))
		{
			fclose(userFile);
			return TRUE;
		}
		
		#ifdef DEBUG_1
			printf("username <%s>		password <%s>\n", user.userName, user.password);
		#endif
	}
	
	fclose(userFile);
	return login;
}
 
 
/************************************************************************
 * 																		*
 * sistem kullanıcı adı ve sifreyi terminalden alarak basliyor			*
 * ancak terminal girisi olmazsa ya da yanlış giriş olduysa tekrar 		*
 * kullanici adi sifre giriş yapmak için kullanılan fonksiyon			*
 * 																		*
 * curr_user_t * structure ı alır ve bilgileri bu structure ın..		*
 * ..üyelerine kaydeder													*
 *																		*
 ***********************************************************************/	

void getAccessInfo(curr_user_t *currentUser)
{
	printf("\nEnter Username\n");
	scanf("%s", currentUser->userName);
	printf("Enter Password\n");
	scanf("%s", currentUser->password);
	printf("\n");
}

/************************************************************************
 * 																		*
 * program içersinde komut almaya yarayan fonksiyon						*
 * int type parametresi ile kullanıcı tipini alır						*
 * kullanıcı tipleri makro olarak belirtilmiştir(STUDENT - INSTRUCTOR)	*
 * 																		*
 * int command ı return eder (komut)									*
 * yanlis komut girisinde dogru girilene kadar istemeye devam eder		*
 ***********************************************************************/
int  getCommand(int type)
{
	char temp[NM_SN_SIZE];
	int command, flag=0, intControl;
	
	printf("\n");
	do
	{		
		scanf("%s", temp);
		command = atoi(temp);
		switch(type)
		{
			case STUDENT:
							if(command < 6 && command > 0)
								flag = PASS;
							else
								printf("Entered wrong command value. TRY AGAIN..\n");
								fseek(stdin, 0, SEEK_END);
							break;
			case INSTRUCTOR:
							if(command < 10	&& command > 0)
								flag = PASS;
							else
								printf("Entered wrong command value. TRY AGAIN..\n");
								fflush(stdin);
							
							break;
		}
		
		
		
	}while(flag != PASS);	
	printf("\n");
	return command;
}

/************************************************************************
 * 																		*
 * string olarak alınacak komutları alır								*
 * input olarak commandID karakter array i alır							*
 * 																		*
 ***********************************************************************/
void getIDCommand(char *commandID)
{
	printf("\nEnter your command(as ID)..\n");
	scanf("%s", commandID);	
	printf("\n");
}


 /***********************************************************************
  * 																	*
  * giriş yapan kullanıcının tüm bilgilerine ulaşır						*
  * bunları curr_user_t tipindeki structure a kaydeder					*
  * 																	*
  * parametre olarak curr_user_t tipinde structure pointer alır			*
  * 																	*
  **********************************************************************/
void findCurrentUserInfo(curr_user_t *currentUser)
{
	FILE *userFile;
	users_t *userArr;
	int size, i;
	
	userFile = fopen("Users.bin", "rb");

	
	fseek(userFile, -sizeof(int), SEEK_END);
	fread(&size, sizeof(int), 1, userFile);
	rewind(userFile);
	
	userArr = malloc(sizeof(users_t)*size);
	
	fread(userArr, sizeof(users_t), size, userFile);
	
	for(i=0; i < size; i++)
	{
		if(!strcmp(userArr[i].userName, currentUser->userName))
		{
			strcpy(currentUser->ID, userArr[i].ID);
			strcpy(currentUser->type, userArr[i].type);
			free(userArr);
			break;	
		}
	}
	
	
	if(!strcmp(currentUser->type, "student"))
	{
		student_t student;
		FILE *studentFile;
		char filePath[PATH_SIZE];
	
		sprintf(filePath,"Students/%s.bin", currentUser->ID);
		studentFile = fopen(filePath, "rb");
		fread(&student, sizeof(student_t), 1, studentFile);
	
		strcpy(currentUser->name, student.name);
		strcpy(currentUser->surname, student.surname);
	
		fclose(studentFile);	
	}
	else if(!strcmp(currentUser->type, "instructor"))
	{
		instructors_t instructor;
		FILE *instructorFile;
		char filePath[PATH_SIZE];
	
		sprintf(filePath,"Instructors/%s.bin", currentUser->ID);
		instructorFile = fopen(filePath, "rb");
	
		fread(&instructor, sizeof(instructors_t), 1, instructorFile);
	
		strcpy(currentUser->name, instructor.name);
		strcpy(currentUser->surname, instructor.surname);
	
		fclose(instructorFile);	
	}
	
	fclose(userFile);
}

/************************************************************************
 * 																		*
 * dosyalarla ilgili kontrol fonksiyonlarının wrapper fonksiyonu		*
 * input yok, dosya yoksa FILE_ERROR, islem basariliysa 0 return eder	*
 * 																		*
 ***********************************************************************/
int fileControls(void)
{
	
	if(checkAllFiles() == FILE_ERROR)
	{
		printf("ERROR !! THERE IS NO DATABASE FILE\n");
		return FILE_ERROR;
	}

	if(checkBinaryFiles() == FALSE)
	{
		writeAllInstructors();
		writeAllUsers();
		writeAllStudent();
		writeAllCourses();
		printf("binary yok");
	}
	
	return 0;
}


 /***********************************************************************
  * 																	*
  *  checkBinaryFiles() ve checkCsvFiles() ı kullanarak..				*
  * .. her 2 dosya türü içinde kontrol yapar							*
  * dosyalar türlerinden en az 1i mevcutsa TRUE							*
  * hiç biri yoksa FILE_ERROR hata kodu return eder.					*
  * 																	*
  **********************************************************************/
int checkAllFiles()
{ 
	int status = TRUE;
	
	if(checkBinaryFiles() == FALSE)
	{
		if(checkCsvFiles() == FALSE)
		{
			status = FILE_ERROR;
		}
	}
	return status;
} 
 
 
  /**********************************************************************
  *																		*
  * bulunulan dizinde databese(.bin) dosyalarının..						*
  * varlığını kontrol eder												*
  * hepsi varsa TRUE herhangi biri eksikse FALSE return eder			*
  * 																	*
  **********************************************************************/
int checkBinaryFiles()
{
		FILE *control;
	int status = TRUE;
	
	control = fopen("Students.bin","rb");
	if(control == NULL)
		status = FALSE;
	else
		fclose(control);
		
	control = fopen("Instructors.bin", "rb");
	if(control == NULL)
		status = FALSE;
	else
		fclose(control);
		
	control = fopen("Courses.bin", "rb");
	if(control == NULL)
		status = FALSE;
	else
		fclose(control);
	
	control = fopen("Users.bin","rb");
	if(control == NULL)
		status = FALSE;
	else
		fclose(control);
			
	return status;
}
 
 
 /***********************************************************************
  *																		*
  * bulunulan dizinde başlangıç csv dosyalarının..						*
  * varlığını kontrol eder												*
  * hepsi varsa TRUE herhangi biri eksikse FALSE return eder			*
  * 																	*
  **********************************************************************/
 
int checkCsvFiles()
{
	FILE *control;
	int status = TRUE;
	
	control = fopen("Students.csv","r");
	if(control == NULL)
		status = FALSE;
	else
		fclose(control);
		
	control = fopen("Instructors.csv", "r");
	if(control == NULL)
		status = FALSE;
	else
		fclose(control);
		
	control = fopen("Courses.csv", "r");
	if(control == NULL)
		status = FALSE;
	else
		fclose(control);
	
	control = fopen("Users.csv","r");
	if(control == NULL)
		status = FALSE;
	else
		fclose(control);
			
	return status;
}


 /***************************************************************************
  *																			*
  * verilen dosyadan öğrenci okuması yapan  fonksiyon						*
  * her seferinde 1 okuma yapar												*
  * student_t *student structure pointer ı vestudentFile file pointer ı alır*
  * dosya sonu olup olmadığını gösteren bir char *status return eder		*
  * 																		*
  * ************************************************************************/
char *readStudents(student_t *student, FILE *studentFile)
{
	char temp[PATH_SIZE*2];
	char *status;
	
	if( (status = fgets(temp, PATH_SIZE*2, studentFile)) != NULL)
	{
	
		strcpy(student->tc, strtok(temp, ","));
		strcpy(student->name, strtok(NULL, ","));
		strcpy(student->surname, strtok(NULL, ","));
		strcpy(student->no, strtok(NULL, ","));
		sscanf(strtok(NULL, ","), "%d", &student->startYear);
		student->courseCount = 0;
		#ifdef DEBUG_STUDENT
			printf(".%s. .%s. .%s.\n", student->tc, student->name, student->no);
		#endif
	}
	return status;
}


  /**************************************************************************
  * 																		*
  * readStudents ı kullanarak Students.csv den tüm öğrencileri okur			*
  * tüm bu bilgileri Students.bin dosyasının içerisine kaydeder				*
  * ayrıca her öğrenci için öğrenci ID(TC) isminde binary dosyası oluşturur	*
  * 																		*
  **************************************************************************/
void writeAllStudent()
{
	char file[PATH_SIZE], *status;
	student_t student;
	FILE *studentFile, *studentBinFile, *temp;
	
	studentFile = fopen("Students.csv", "r");
	studentBinFile = fopen("Students.bin", "wb");
	
	system("mkdir Students");
	do
	{
		status = readStudents(&student, studentFile);
		if(status != NULL)
		{
			fwrite(&student, sizeof(student_t), 1, studentBinFile);
			sprintf(file, "Students/%s.bin", student.tc);
			temp = fopen(file, "wb");
			fwrite(&student, sizeof(student_t), 1, temp);
			fclose(temp);
		}

	}while(status != NULL);
	
	fclose(studentFile);
	fclose(studentBinFile);
}


 /***************************************************************************
  *																			*
  * verilen dosyadan eğitimci okuması yapan  fonksiyon						*
  * her seferinde 1 okuma yapar												*
  * instructors_t *instructor structure pointer ı ve..						*
  * ..instructorFile file pointer ı alır									*
  * dosya sonu olup olmadığını gösteren bir char *status return eder		*
  * 																		*
  * ************************************************************************/
char *readInstructors(instructors_t *instructor, FILE *instructorFile)
{
	char temp[PATH_SIZE*2];
	char *status;
	
	if( (status = fgets(temp, PATH_SIZE*2, instructorFile)) != NULL)
	{
		temp[strlen(temp) - 2] = '\0';
		strcpy(instructor->tc, strtok(temp, ","));
		strcpy(instructor->name, strtok(NULL, ","));
		strcpy(instructor->surname, strtok(NULL, ","));
					
		#ifdef DEBUG_INSTRUCTOR
			printf(".%s. .%s. .%s.\n", instructor->tc, instructor->name, instructor->surname);
		#endif
	}
	return status;
}


  /**************************************************************************
  * 																		*
  * readInstructors ı kullanarak Instructors.csv den tüm eğitimcileri okur	*
  * tüm bu bilgileri Instructors.bin dosyasının içerisine kaydeder			*
  * ayrıca her eğitimci için eğitimci TCNO isminde binary dosyası oluşturur	*
  * 																		*
  **************************************************************************/
void writeAllInstructors()
{
	char file[PATH_SIZE], *status;
	instructors_t instructor;
	FILE *instructorFile, *instructorBinFile, *temp;
	
	instructorFile = fopen("Instructors.csv", "r");
	instructorBinFile = fopen("Instructors.bin", "wb");
	
	system("mkdir Instructors");
	do
	{
		status = readInstructors(&instructor, instructorFile);
		if(status != NULL)
		{
			fwrite(&instructor, sizeof(instructors_t), 1, instructorBinFile);
			sprintf(file, "Instructors/%s.bin", instructor.tc);
			temp = fopen(file, "wb");
			fwrite(&instructor, sizeof(instructors_t), 1, temp);
			fclose(temp);
		}

	}while(status != NULL);
	
	fclose(instructorFile);
	fclose(instructorBinFile);
}


 /***************************************************************************
  *																			*
  * verilen dosyadan ders okuması yapan  fonksiyon							*
  * her seferinde 1 okuma yapar												*
  * courses_t *course structure pointer ı ve *courseFile file pointer ı alır*
  * dosya sonu olup olmadığını gösteren bir char *status return eder		*
  * 																		*
  * ************************************************************************/
char *readCourses(courses_t *course, FILE *courseFile)
{
	FILE *instFile;
	char temp[PATH_SIZE*2], *temp2;
	char *status;
	int i = 0;
	instructors_t instructor;
	char filePath[PATH_SIZE];
	
	if( (status = fgets(temp, PATH_SIZE*2, courseFile)) != NULL && temp[0] != '\0')
	{
		temp[strlen(temp)-2] = '\0';
		strcpy(course->ID, strtok(temp, ","));
		strcpy(course->name, strtok(NULL, ","));
		strcpy(course->instructorID, strtok(NULL, ","));
		
		sprintf(filePath, "Instructors/%s.bin", course->instructorID);
		
		instFile = fopen(filePath, "rb");
		
		fread(&instructor, sizeof(instructors_t), 1, instFile);
		
		fclose(instFile);
		
		
		strcpy(course->instructorName, instructor.name);
		strcpy(course->instructorSurname, instructor.surname);
		course->isOpen = NO;
		course->isAvailable = NO;
		course->stdCount = 0;
		do
		{	temp2 = strtok(NULL, ",");
		
			if( temp2 != NULL)
			{	
				
				strcpy(course->preq[i].ID, temp2);
				#ifdef DEBUG_COURSE_PREQS
					printf("..%s >>\n", course->preq[i].ID);fflush(stdout);
				#endif
				i++;
			}
		
		}while(temp2 != NULL);

		#ifdef DEBUG_COURSE
			printf(".%s. .%s. .%s.\n", course->ID, course->name, course->instructorSurname);
		#endif
			
	}
	course->preqNum = i;
	return status;
}


  /**************************************************************************
  * 																		*
  * readCourses fonskiyonunu kullanarak Courses.csv den tüm dersleri okur	*
  * tüm bu bilgileri Courses.bin dosyasının içerisine kaydeder				*
  * ayrıca her ders için dersin ID si isminde bir binary dosyası oluşturur	*
  * 																		*
  **************************************************************************/
void writeAllCourses()
{
	char file[PATH_SIZE], *status;
	courses_t course;
	int counter=0;
	FILE *courseFile, *courseBinFile, *temp;
	
	courseFile = fopen("Courses.csv", "r");
	courseBinFile = fopen("Courses.bin", "wb");
	
	system("mkdir Courses");
	do
	{
		status = readCourses(&course, courseFile);
		if(status != NULL)
		{
			
			fwrite(&course, sizeof(courses_t), 1, courseBinFile);
			sprintf(file, "Courses/%s.bin", course.ID);
			temp = fopen(file, "wb");
			fwrite(&course, sizeof(courses_t), 1, temp);
			fclose(temp);
			counter++;
		}

	}while(status != NULL);-
	
	fwrite(&counter, sizeof(int), 1, courseBinFile);
	
	fclose(courseFile);
	fclose(courseBinFile);
}


 /***************************************************************************
  *																			*
  * verilen dosyadan kullanıcı okuması yapan  fonksiyon						*
  * her seferinde 1 okuma yapar												*
  * users_t *user structure pointer ı ve userFile file pointer ı alır		*
  * dosya sonu olup olmadığını gösteren bir char *status return eder		*
  * 																		*
  * ************************************************************************/
char *readUsers(users_t *user, FILE *userFile)
{
	char temp[PATH_SIZE*2];
	char *status;
	
	if( (status = fgets(temp, PATH_SIZE*2, userFile)) != NULL)
	{
		temp[strlen(temp)-2] = '\0';
		strcpy(user->ID, strtok(temp, ","));
		strcpy(user->type, strtok(NULL, ","));
		strcpy(user->userName, strtok(NULL, ","));
		strcpy(user->password, strtok(NULL, ","));
			
		#ifdef DEBUG_USER
			printf(".%s. .%s. .%s. .%s.\n", user->ID, user->type, user->userName, user->password);
		#endif
			
	}
	return status;
}


  /**************************************************************************
  * 																		*
  * readUsers fonskiyonunu kullanarak Users.csv den tüm kullanıcıları okur	*
  * tüm bu bilgileri Users.bin dosyasının içerisine kaydeder				*
  * 																		*
  **************************************************************************/
void writeAllUsers()
{
	char file[PATH_SIZE], *status;
	users_t user;
	FILE *userFile, *userBinFile;
	int counter=0;
	
	userFile = fopen("Users.csv", "r");
	userBinFile = fopen("Users.bin", "wb");
	
	do
	{
		status = readUsers(&user, userFile);
		if(status != NULL)
			fwrite(&user, sizeof(users_t), 1, userBinFile);

		counter++;		
	}while(status != NULL);
	
	fwrite(&counter, sizeof(int), 1, userBinFile);
	
	fclose(userFile);
	fclose(userBinFile);
}



 /***************************************************************************
 * 																			*
 * ders eklenmeye müsait olan tüm dersleri listelenir						*
 * eğer açık olan ders yoksa ekrana hata mesajı yazılır						*
 * 																			*
 * *************************************************************************/
int listCourses4Add(void)
{
	courses_t *courseArr;
	FILE *courseFile;
	int size=0, i=0, counter=0;
	int status = TRUE;
	
	courseFile = fopen("Courses.bin", "rb");
	
	fseek(courseFile, -sizeof(int), SEEK_END);
	fread(&size, sizeof(int), 1, courseFile);
	rewind(courseFile);
	courseArr = malloc(sizeof(courses_t)*size);
	
	fread(courseArr, sizeof(courses_t), size, courseFile);

	printf("	ID	NAME			INSTRUCTOR		OPEN/CLOSE	ADD/RMV MODE\n");
	for(i=0; i < size; ++i)
	{
		if(courseArr[i].isOpen && courseArr[i].isAvailable)
		{
			printf("%d. %s\t%s\t\t%s ", i+1, courseArr[i].ID, courseArr[i].name, courseArr[i].instructorName);
			printf("%s   \t%d  \t\t%d\n", courseArr[i].instructorSurname,courseArr[i].isOpen, courseArr[i].isAvailable);
			counter++;
		}
	}
	
	if(!counter)
	{
		printf("Any course is not open yet ! \n\n");
		status = FALSE;
	}
	else
		printf("\n");
	
	
	free(courseArr);
	fclose(courseFile);
	
	return status;
}


/****************************************************************************
 * 																			*
 * girilen dersin var olup olmadığını varsa da açılmış ve ders eklemeye..	*
 * ..müsait olup olmadığının kontrolunu yapar.								*
 * input parametre olarak dersin ID sini(courseID) alır						*
 * ders yoksa USER_COURSE_ERR												*
 * var ama açık ya da ekleme süresi içinde değilse FALSE return eder.		*
 *																			*
 * **************************************************************************/
int checkAvailableOpen(const char *courseID)
{
	FILE *courseFile;
	courses_t course;
	int status;
	char filePath[PATH_SIZE];
	
	sprintf(filePath, "Courses/%s.bin", courseID);
	courseFile = fopen(filePath, "rb");
	
	if(courseFile == NULL)
		return USER_COURSE_ERR;
	
	fread(&course, sizeof(courses_t), 1, courseFile);
	
	
	if(course.isOpen && course.isAvailable)
		status = TRUE;
	else 
		status = FALSE;
	
	fclose(courseFile);
	return status;	
}

/************************************************************************
 * 																		*
 * alinacak dersin onsart durumunu kontrol eder							*
 * studentID ve courseID input olarak alır								*
 * onsart saglanıyorsa TRUE yoksa FALSE return eder						*
 * 																		*
 ***********************************************************************/
int preqControl(const char *studentID, const char *courseID)
{
	int preqFlag = 0, realFlag=0, status;
	int i, j;
	int oldCourseCount;
	FILE *oldFile;
	FILE *courseFile;
	char filePath[PATH_SIZE];
	std_crs_t *oldCourses;
	courses_t course;
	
	
	sprintf(filePath, "Courses/%s.bin", courseID);
	courseFile = fopen(filePath, "rb");
	
	fread(&course, sizeof(courses_t), 1, courseFile);
	
	if(course.preqNum == 0)
	{
		fclose(courseFile);
		return TRUE;
	}
	
	sprintf(filePath, "Students/%s_old.bin", studentID);
	if((oldFile = fopen(filePath, "rb")) == NULL)
	{
		fclose(courseFile);
		return FALSE;
	}
	
	fread(&oldCourseCount, sizeof(int), 1, oldFile);
	
	if(oldCourseCount < course.preqNum) 
	{
		fclose(courseFile);
		fclose(oldFile);
		return FALSE;
	}
	
	oldCourses = malloc(sizeof(std_crs_t) * oldCourseCount);
	
	fread(oldCourses, sizeof(std_crs_t), oldCourseCount, oldFile);
	
	
	
	for(j=0; j < course.preqNum; j++)
	{
		for(i=0; i < oldCourseCount; i++)
		{
			if(strcmp(oldCourses[i].ID, course.preq[j].ID) == 0)
			{ 
				preqFlag++;
			}
		}
		if(preqFlag)
			realFlag++;
			
		preqFlag = 0;
	}
	
	if(realFlag == course.preqNum)
		status = TRUE;
	else 
		status = FALSE;
	
	free(oldCourses);
	fclose(courseFile);
	fclose(oldFile);
	
	return status;	
}

 /***************************************************************************
 *																			*
 * Ders ekleme çıkarma işlemini yapan fonksiyon								*
 * char *courseID ve char *studentID paramatre olarak alır					*
 * hem öğrencinin hem de dersin dosyasına karşılıklı bilgileri kopyalar		*
 * bilgi girişi yanlışsa dosya açılamaz ve USER_COURSE_ERR return edilir	*
 * aynı ders 2 kere alınmak isteniyorsa da hata verilir						*
 * 																			*
 ***************************************************************************/
int enrolCourse(const char *courseID, const char *studentID)
{
	FILE *courseFile, *studentFile;
	char filePath[PATH_SIZE];
	crs_std_t student;
	std_crs_t course, temp;
	courses_t realCourse;
	student_t realStudent;
	

	sprintf(filePath, "Courses/%s.bin", courseID);
	courseFile = fopen(filePath, "rb+");
	sprintf(filePath, "Students/%s.bin", studentID);
	studentFile = fopen(filePath, "rb+");
	
	fread(&realCourse, sizeof(courses_t), 1, courseFile);
	fread(&realStudent, sizeof(student_t), 1, studentFile);
	
	
	while(!feof(studentFile))
	{
		fread(&temp, sizeof(std_crs_t), 1, studentFile);
		if(!strcmp(courseID, temp.ID))
		{
			fclose(courseFile);
			fclose(studentFile);
			return USER_COURSE_ERR;
		}
	}

	realCourse.stdCount += 1;
	rewind(courseFile);
	fwrite(&realCourse, sizeof(courses_t), 1, courseFile);
	
	strcpy(student.ID, realStudent.tc);
	strcpy(student.name, realStudent.name);
	strcpy(student.surname, realStudent.surname);
	student.grade = -1.0;
	
	
	fseek(courseFile, 0, SEEK_END);
	fwrite(&student, sizeof(crs_std_t), 1, courseFile);
	
	
/******************************************************************************/	
	
	strcpy(course.ID, realCourse.ID);
	strcpy(course.name, realCourse.name);
	course.grade = -1.0;
	

	#ifdef DEBUG_ENROL
	printf("%s %s %d\n", course.ID, course.name, course.grade );
	#endif
	
	fseek(studentFile, 0, SEEK_END);
	fwrite(&course, sizeof(std_crs_t), 1, studentFile);
	realStudent.courseCount += 1;
	rewind(studentFile);
	fwrite(&realStudent, sizeof(student_t), 1, studentFile);

	#ifdef DEBUG_ENROL
	fseek(studentFile, sizeof(student_t), SEEK_SET);
	fread(&course, sizeof(std_crs_t), 1, studentFile);
	printf("%s %s %d\n", course.ID, course.name, course.grade );
	#endif
	
	printf("Your enrolled to \"%s\"\n\n", courseID);
	
	fclose(courseFile);
	fclose(studentFile );
	
	return 0;
}



/****************************************************************************
 * 																			*
 * Öğrencinin aldığı dersleri listeleyen ve istenirse kaydeden fonskiyon 	*
 * char *studentID: Öğrenci ID'si alır(TC kimlik Numarası)					*
 * int saveMode : kayıt isleminin sorulup sorulmayacagini kontrol eder		*
 *																			*
 ***************************************************************************/
int listAndSaveEnrolledCourses(const char *studentID, int saveMode)
{
	FILE *studentFile;
	char filePath[PATH_SIZE];
	std_crs_t *course;
	student_t student;
	int i;
	char saveCom;
	
	sprintf(filePath, "Students/%s.bin", studentID);
	studentFile = fopen(filePath, "r");
	
	
	fread(&student, sizeof(student_t), 1, studentFile);
		
	#ifdef DEBUG_COUNTERS
		printf("%d ", student.courseCount);
	#endif
	
	if(student.courseCount == 0)
	{	
		fclose(studentFile);
		printf("There is no course you enrolled\n");
		return 1;
	}
	
	course = malloc(sizeof(std_crs_t) * student.courseCount);
	fread(course, sizeof(std_crs_t), student.courseCount, studentFile);
	
	printf("    ID	\tNAME		\tGRADE\n");
	for(i=0; i < student.courseCount; i++)
		printf("%d. %s \t%s \t\t%.2f\n", i+1, course[i].ID, course[i].name, course[i].grade );
	
	if(saveMode == YES)
	{	
		printf("\n\nDo you want to save this grades ?? \n [Y] or [N]..\n");
		scanf(" %c", &saveCom);	
		
		if(saveCom == 'Y' || saveCom == 'y')
		{
			FILE *gradeOutput;
			sprintf(filePath, "%s_grades.txt", student.no);
			gradeOutput = fopen(filePath, "w");
			fprintf(gradeOutput, "%s %s\n\n", student.name, student.surname);
			fprintf(gradeOutput, "ENROLLED COURSES GRADES\n\n");
			for(i=0; i < student.courseCount; i++)
			{
				fprintf(gradeOutput,"%d. %s >> %s\t %.2f\n", i+1, course[i].ID, course[i].name, course[i].grade );
			}
			fclose(gradeOutput);
			
			printf("\nThe grades have been saved to \"%s.bin\"", student.no);
		}
		else if(saveCom != 'N' && saveCom != 'n')
			printf("Process was been unsuccess\n\n");
	}
	printf("\n");
	free(course);
	fclose(studentFile);
	return 0;
}
/******************************************************************************/


/****************************************************************************
 * 																			*
 * ders kaydı silme islemini yapar											*
 * char *courseID, char *studentID, ve byWho integer paramatresi alır		*
 * int byWho : silme islemini kimin yapacagıdır, eger ogrenci yapacaksa..	*
 *.. o ogrenci ile alakalı islem yapılır, dersin ve ogrencinin dosyasindan..*
 *.. tum bilgiler silinir.													*
 * 		**		**		**		**		**		**		**		**		**	*
 * eger islemi egitimci yapiyorsa ogrenci durumunda yapilanlara ek olarak..	*
 * ..ogrenciID si ile eski dersler dosyası olusturulur ve bu ders kaydedilir*
 *																			*
 ***************************************************************************/
int disenrolCourse(const char *courseID, const char *studentID, int byWho)
{
	FILE *courseFile, *studentFile;
	char filePath[PATH_SIZE];
	int i=0, j=0, count;
	int counter=0;
	crs_std_t *student;
	std_crs_t *course, temp;
	courses_t realCourse;
	student_t realStudent;
	
		
	sprintf(filePath, "Students/%s.bin", studentID);
	studentFile = fopen(filePath, "rb+");
	if(studentFile == NULL)
		return USER_COURSE_ERR;
	
		
	fread(&realStudent, sizeof(student_t), 1, studentFile);
	course = malloc(sizeof(std_crs_t) * realStudent.courseCount);
	
	fread(course, sizeof(std_crs_t), realStudent.courseCount, studentFile);
	
	for(i=0; i < realStudent.courseCount; i++)
	{
		if(!strcmp(courseID, course[i].ID))
			counter++;
	}
		
	if(counter == 0)
	{
		free(course);
		fclose(studentFile);
		return USER_COURSE_ERR;
	}
	#ifdef DEBUG_DISENROL
		for(i=0; i < realStudent.courseCount; i++){
			printf("%s\n", course[i].ID);
		}
	#endif
	
	count = realStudent.courseCount;
	realStudent.courseCount -= 1;
	fclose(studentFile);
	
	sprintf(filePath, "Students/%s.bin", studentID);
	studentFile = fopen(filePath, "wb");
	
	fwrite(&realStudent, sizeof(student_t), 1, studentFile);
	
	for(j=0; j < count; j++)
	{
		if(strcmp(course[j].ID, courseID) != 0)
		{ 
			fwrite(&course[j], sizeof(std_crs_t), 1, studentFile);
		}
	}
	
	
/************************/	
	if(byWho == INSTRUCTOR)
	{
		std_crs_t oldCourse, temp;
		FILE *oldFile;
		sprintf(filePath, "Students/%s_old.bin", studentID);
		int sameFlag = 0, oldCount=1;
	
		if((oldFile = fopen(filePath, "rb+")) == NULL)
		{
			oldFile = fopen(filePath, "ab");
			fwrite(&oldCount, sizeof(int), 1, oldFile);
			
			for(j=0; j < count; j++)
			{
				if(strcmp(course[j].ID, courseID) == 0)
				{ 
					fwrite(&course[j], sizeof(std_crs_t), 1, oldFile);
				}
			}

		}
		else
		{ 
			fread(&oldCount, sizeof(int), 1, oldFile);
			for(j=0; j < count; j++)
			{
				if(strcmp(course[j].ID, courseID) == 0)
				{ 
					temp = course[j];
				}
			}
			
			fseek(oldFile, sizeof(int), SEEK_SET);
			for(j=0; j < oldCount; j++)
			{
				fread(&oldCourse, sizeof(std_crs_t), 1, oldFile);
				if(strcmp(temp.ID, oldCourse.ID) == 0)
				{
					fseek(oldFile, -sizeof(std_crs_t), SEEK_CUR);
					fwrite(&temp, sizeof(std_crs_t), 1, oldFile);
					sameFlag++;			
				}
			}
			
			if(sameFlag == 0)
			{
				fseek(oldFile, 0, SEEK_END);
				fwrite(&temp, sizeof(std_crs_t), 1, oldFile);
				oldCount++;
			}
			
			rewind(oldFile);
			fwrite(&oldCount, sizeof(int), 1, oldFile);
		}
		
		fclose(oldFile);
	}
	
	fclose(studentFile);
	/****************************************************************************/
	
	sprintf(filePath, "Courses/%s.bin", courseID);
	courseFile = fopen(filePath, "rb+");
	if(courseFile == NULL)
		return USER_COURSE_ERR;
	
	fread(&realCourse, sizeof(courses_t), 1, courseFile);
	count = realCourse.stdCount;
	
	student = malloc(sizeof(crs_std_t) * count);
	
	fread(student, sizeof(crs_std_t), count, courseFile);

	
	fclose(courseFile);
	
	sprintf(filePath, "Courses/%s.bin", courseID);
	courseFile = fopen(filePath, "wb");
	
	realCourse.stdCount -=1;
	fwrite(&realCourse, sizeof(courses_t), 1, courseFile);
	
	for(j=0; j < count; j++)
	{
		if(strcmp(student[j].ID, studentID) != 0)
		{ 
			fwrite(&student[j], sizeof(crs_std_t), 1, courseFile);
		}
	}
	
	free(student);
	free(course);
	fclose(courseFile);
	return 0;
}


/****************************************************************************
 * 																			*
 * Ogrenci transkripti olusturan fonksiyon									*
 * input parametre olarak userID(studentID) alir.							*
 * ogrencinin kendi dosyasi ve mezun oldugu derslerin dosyasindan..			*
 * .. ders ve kisisel bilgileri alir										*
 * 																			*
 ***************************************************************************/
int createTranscript(const char *userID)
{
	char filePath[PATH_SIZE];
	FILE *oldFile, *studentFile, *transcriptFile;
	int courseCount, i;
	std_crs_t *courses;
	student_t student;
	double total=0;
	
	sprintf(filePath, "Students/%s_old.bin", userID);
		
	if((oldFile = fopen(filePath, "rb")) == NULL)
		return USER_COURSE_ERR;
	
	sprintf(filePath, "Students/%s.bin", userID);
	studentFile = fopen(filePath, "rb");
	fread(&student, sizeof(student_t), 1, studentFile);
	
	sprintf(filePath, "%s_transcript.txt", userID);
	transcriptFile = fopen(filePath, "w");
	
	fread(&courseCount, sizeof(int), 1, oldFile);
	courses = malloc(sizeof(std_crs_t) * courseCount);
	
	fread(courses, sizeof(std_crs_t), courseCount, oldFile);
	
	time_t t;
	time(&t);

	fprintf(transcriptFile,"\n%s\n\n",ctime(&t));
	
	fprintf(transcriptFile, "Name:\t\t\t%s\nSurname:\t\t%s\nStudent Number:\t%s\nStart Year:\t\t%d\n\n\n", student.name, student.surname, student.no, student.startYear);
	fprintf(transcriptFile, "------------------------------------------------------\n\n");
	
	for(i=0; i < courseCount; i++)
	{
		fprintf(transcriptFile, "%d. %s %s %.2f\n", i+1, courses[i].ID, courses[i].name, courses[i].grade);		
		total += courses[i].grade;
	}
	
	fprintf(transcriptFile,"\n\n------------------------------------------------------");
	fprintf(transcriptFile, "\n\nYour courses averages is : %.2f", total / courseCount);


	
	free(courses);
	fclose(transcriptFile);
	fclose(studentFile);
	fclose(oldFile);
	return 0;
}


 /***************************************************************************
 *																			* 
 * Eğitimcinin yapacağı işleme göre ekrana ders adlarını yazar				*
 * Input Paramatreler:														*
 * char *instructorName, char *instructorSurname : Eğitimci adı ve soyadı	*
 * int forWhat: fonskiyonun yapacağı işi tanımlar, makro olarak yazılmıştır *
 * 				BEGIN: ders açılacağını -- FINAL: dersin bitirileceğini		*
 * 				GRADE: not verme işlemi yapılacağını belirtir. 				*
 *																			*
 * int saveMode: kayıt sorusunun sorulup sorulmayacagini belirtir			*
 * 																			*
 ***************************************************************************/
int listCourses4Instructor(const char *instructorID, int forWhat, int saveMode)
{
	courses_t *courseArr;
	char saveCom;
	FILE *courseFile;
	int size=0, i=0, counter=0;
	
	courseFile = fopen("Courses.bin", "rb");
	
	fseek(courseFile, -sizeof(int), SEEK_END);
	fread(&size, sizeof(int), 1, courseFile);
	rewind(courseFile);
	
	courseArr = malloc(sizeof(courses_t)*size);
	
	fread(courseArr, sizeof(courses_t), size, courseFile);

	if(forWhat == BEGIN)
		printf("ID	\tNAME	\t     OPEN/CLOSE	 ADD/RMV\n");
	else
		printf("ID		\tNAME\n");
	
	for(i=0; i < size; ++i)
	{
			if(!strcmp(courseArr[i].instructorID, instructorID) && forWhat == BEGIN)
			{
				printf("%s\t\t%s\t\t%d\t    %d\n", courseArr[i].ID, courseArr[i].name, courseArr[i].isOpen, courseArr[i].isAvailable);
				counter++;
			}	
		
			if(!strcmp(courseArr[i].instructorID, instructorID) &&  courseArr[i].isOpen && forWhat == IF_OPEN)
			{
				printf("%s\t\t\t%s\n", courseArr[i].ID, courseArr[i].name);
				counter++;
			}
			
			if(!strcmp(courseArr[i].instructorID, instructorID) &&  courseArr[i].isOpen && !courseArr[i].isAvailable && forWhat == AVAILABLE)
			{
				printf("%s\t\t\t%s\n", courseArr[i].ID, courseArr[i].name);
				counter++;
			}
			
			if(!strcmp(courseArr[i].instructorID, instructorID) &&  courseArr[i].isOpen && courseArr[i].isAvailable && forWhat == NAVAILABLE)
			{
				printf("%s\t\t\t%s\n", courseArr[i].ID, courseArr[i].name);
				counter++;
			}
			
	}
	
	if(!counter)
	{
		printf("There is no course as you want\n");
		free(courseArr);
		fclose(courseFile);
		return FALSE;
	}
	
	if(forWhat == IF_OPEN && saveMode == YES)
	{
		printf("-----------------------------------------\n\n");
		printf("\n\nDo you want to save this courses ?? \n [Y] or [N]..\n");
		scanf(" %c", &saveCom);	
		
		if(saveCom == 'Y' || saveCom == 'y')
		{
			saveOpenCourses(instructorID, courseArr, size);
			
		}
	}
	free(courseArr);
	fclose(courseFile);
	return TRUE;
}


/****************************************************************************
 * 																			*
 * egitimcinin acik olan derslerini kaydeder								*
 * instructorID ve courses_ tipinde derslerin arrayini ve buyuklugunu alır	*
 * 																			*
 * egitimciAdi_soyadi_opencourses.csv olarak dosya acar ve kayıt yapar		*
 * 																			*
 * *************************************************************************/
void saveOpenCourses(const char *instructorID, courses_t *courses, int size)
{
	
	FILE *courseOut, *instructorFile;
	char filePath[PATH_SIZE];
	instructors_t instructor;
	int i;

	sprintf(filePath, "Instructors/%s.bin", instructorID);
	instructorFile = fopen(filePath, "rb");
	
	fread(&instructor, sizeof(instructors_t), 1, instructorFile);	
	
	sprintf(filePath, "%s_%s_openCourses.csv", instructor.name, instructor.surname);
	courseOut = fopen(filePath, "w");
	
	for(i=0; i < size; ++i)
	{
		if(!strcmp(courses[i].instructorID, instructorID) &&  courses[i].isOpen)
		{
			fprintf(courseOut, "%s,%s,%s,%s\n", courses[i].ID, courses[i].name, instructor.name, instructor.surname);
		}
	}

	printf("\nThese courses have been saved to \"%s_%s_openCourses.csv\"	\n", instructor.name, instructor.surname);
	fclose(instructorFile);
	fclose(courseOut);
	
}


/****************************************************************************
 * 																			*
 * not verme ve duzenleme icin dersi alan ogrencileri listeler				*
 * courseID, instructorID(currentUserID) ve int saveMode alir				*
 * saveMode kayıt sorusunun sorulup sorulmayacagini belirtir (YES ve NO)	*
 *																			*
 ***************************************************************************/
int listStudents4Grade(const char *courseID, const char *currentUserID, int saveMode)
{
	FILE *courseFile;
	char filePath[PATH_SIZE], command;
	crs_std_t *students;
	courses_t course;
	int i;
	
	sprintf(filePath, "Courses/%s.bin", courseID);
	courseFile = fopen(filePath, "rb");
	if(courseFile == NULL)
		return USER_COURSE_ERR;
	
	fread(&course, sizeof(courses_t), 1, courseFile);
	
	if(strcmp(course.instructorID, currentUserID))
	{
		fclose(courseFile);
		return USER_COURSE_ERR;
	}
	
	students = malloc(sizeof(crs_std_t) * course.stdCount);
	
	fread(students, sizeof(crs_std_t), course.stdCount, courseFile);

	printf("	ID	\tNAME		SURNAME		GRADE\n");
	for(i=0; i < course.stdCount; i++)
	{
		printf("%d. %s   \t%s   \t\t%s   \t%.2f\n", i+1, students[i].ID, students[i].name, students[i].surname, students[i].grade);
	}
	
	
	if(saveMode == YES && course.stdCount != 0)
	{
		printf("\n\nDo you want to save these grades??\n");
		printf("Enter [Y] or [N] \n");
		scanf(" %c", &command);
		
		if(command == 'Y' || command == 'y')
		{
			saveGrades(&course, students);
			printf("\nThe student(s) has been saved to \"%s_grades.csv\"\n", courseID);
		}
		
	}
	
	free(students);
	fclose(courseFile);
	return course.stdCount;
}


/****************************************************************************
 * 																			*
 * not verme ve duzenleme islemi icin girilecek notu alan fonksiyon			*
 * 1-100 arası not verilebilir. 0 nota dahil degildir						*
 * girilen notu double olarak return eder									*
 * 																			*
 ***************************************************************************/
double getGrade(void)
{
	double grade;
	char temp[PATH_SIZE*2];
	
	printf("Enter a grade (min 1 max 100)");
	printf("(Don't try enter -100)\n");
	
	do
	{
		scanf("%s", temp);
		grade = atof(temp);
	
		if(grade > 100.0 || grade < 1.0)
			printf("Wrong grade value, TRY AGAIN..\n\n");
		
	}while(grade > 100.0 || grade < 1.0);
	
	return grade;
}



/****************************************************************************
 * 																			*
 * girilen notlarin kaydedilme islemini yapar								*
 * dersin structure pointerını ve ogrencilerin bulundugu arrayi alir		*
 * 																			*
 * *************************************************************************/
void saveGrades(courses_t *course, crs_std_t *students)
{
	FILE *gradeOut;
	int i;
	char filePath[PATH_SIZE];
	
	sprintf(filePath, "%s_grades.csv", course->ID);
	gradeOut = fopen(filePath, "w");
	
	
	for(i=0; i < course->stdCount; i++)
	{
		fprintf(gradeOut,"%s,%s,%s,%.2f\n", students[i].ID, students[i].name, students[i].surname, students[i].grade);
		
	}

	fclose(gradeOut);
}


/****************************************************************************
 * 																			*
 * not verme islemini yapan fonksiyon										*
 * ogrenci, egitimci ve o anki kullanicinin IDlerini ve notu alir			*
 * eger ogrenci yoksa, ders yoksa ya da o anki kullanicinin dersi degilse..	*
 * .. USER_COURSE_ERR return eder.											*
 * islem basarili olursa 0 return edilir									*
 *																			*
 ***************************************************************************/
int assignGrade(const char *studentID, const char *courseID, const char *currentUserID, double grade)
{
	FILE *studentFile, *courseFile;
	char filePath[PATH_SIZE];
	crs_std_t student;
	std_crs_t course;
	int i, flag=0;
	student_t realStudent;
	courses_t realCourse;
	
	
	sprintf(filePath, "Students/%s.bin", studentID);
	studentFile = fopen(filePath, "rb+");
	if(studentFile == NULL)
		return USER_COURSE_ERR;
	
	sprintf(filePath, "Courses/%s.bin", courseID);
	courseFile = fopen(filePath, "rb+");
	if(courseFile == NULL)
	{
		fclose(studentFile);
		return USER_COURSE_ERR;
	}
	
	fread(&realCourse, sizeof(courses_t), 1, courseFile);
	
	if(strcmp(currentUserID, realCourse.instructorID))
	{
		fclose(courseFile);
		fclose(studentFile);
		return USER_COURSE_ERR;
	}
	
	for(i=0; i < realCourse.stdCount; i++)
	{
		fread(&student, sizeof(crs_std_t), 1, courseFile);
		if(!strcmp(studentID, student.ID))
		{
			student.grade = grade;
			fseek(courseFile, -sizeof(crs_std_t), SEEK_CUR);
			fwrite(&student, sizeof(crs_std_t), 1, courseFile);
			flag++;
			break;			
		}
	}
	fclose(courseFile);
	
	if(!flag)
	{
		fclose(studentFile);
		return USER_COURSE_ERR;
	}
	
	
	fread(&realStudent, sizeof(student_t), 1, studentFile);
	
	for(i=0; i < realStudent.courseCount; i++)
	{
		
		fread(&course, sizeof(std_crs_t), 1, studentFile);
		if(!strcmp(courseID, course.ID))
		{
			course.grade = grade;
			fseek(studentFile, -sizeof(std_crs_t), SEEK_CUR);
			fwrite(&course, sizeof(std_crs_t), 1, studentFile);
			break;			
		}
	}
	
	fclose(studentFile);
	
	printf("\nGrading is success\n");
	return 0;
}

/****************************************************************************
 * 																			*
 * dersin notlandirilip notlandirilmadigini kontrol eder					*
 * ders, ogrenci, kullanici IDleri yanisira islemin ne icin yapilacagini..	*
 * ..belirten bir integer paramatre(forWhat) alir							*
 * forWhat islemin dersi alan tum ogrenciler icin mii..						*
 * ..yoksa tek bir ogrenci mi icin yapilacagini belirtir					*
 * eger ders yoksa USER_COURSE_ERR return eder.								*
 * islem basarili olursa 0 return edilir									*
 *																			*
 ***************************************************************************/
int isGraded(const char *courseID, const char *studentID, const char *currentUserID, int forWhat)
{
	FILE *courseFile;
	char filePath[PATH_SIZE];
	crs_std_t *students;
	courses_t course;
	int i, gradeCount=0, status;
	
	sprintf(filePath, "Courses/%s.bin", courseID);
	if((courseFile = fopen(filePath, "rb")) == NULL)
		return USER_COURSE_ERR;
	
	fread(&course, sizeof(courses_t), 1, courseFile);
	
	if(strcmp(currentUserID, course.instructorID))
	{
		fclose(courseFile);
		return USER_COURSE_ERR;
	}
	
	students = malloc(sizeof(crs_std_t) * course.stdCount);
	
	fread(students, sizeof(crs_std_t), course.stdCount, courseFile);
	
	
	switch(forWhat)
	{
		case ALL:
					for(i=0; i < course.stdCount; i++)
					{
						if(students[i].grade == -1.0)
							gradeCount++;		
					}
					
					if(gradeCount)
						status = FALSE;
					else
						status = TRUE;
					
					break;
					
		case ONE:
					for(i=0; i < course.stdCount; i++)
					{
						
						if(!strcmp(students[i].ID, studentID))
						{
							
							if(students[i].grade == -1.0)
								status = FALSE;
							else
								status = TRUE;							
						}
					}
					
					break;
	}
	
		
	free(students);
	fclose(courseFile);
	
	return status;
}

/****************************************************************************
 *																			*
 * egitimcinin dersi kapatmasinin saglayan fonksiyon						*
 * dersin ve o anki kullanicinin ID sini alir								*
 * dersi alan tüm ogrenciler icin disnerol islemini yapar ve dersi kapatir	*
 * 																			*
 ***************************************************************************/
void finalizeCourse(const char *courseID, const char *currentUserID)
{
	FILE *courseFile;
	char filePath[PATH_SIZE];
	crs_std_t *students;
	courses_t course;
	int i;
	
	sprintf(filePath, "Courses/%s.bin", courseID);
	courseFile = fopen(filePath, "rb");
	
	fread(&course, sizeof(courses_t), 1, courseFile);
	
	students = malloc(sizeof(crs_std_t) * course.stdCount);
	
	fread(students, sizeof(crs_std_t), course.stdCount, courseFile);
	
	for(i=0; i < course.stdCount; i++)
	{	
		disenrolCourse(courseID, students[i].ID, INSTRUCTOR);
	}
	
	processCourse(courseID, CLOSE, currentUserID);
	
	fclose(courseFile);
}


 /***************************************************************************
 *																			* 
 * derslerin modlarını ayarlayan fonksiyon									*
 * char *courseID ve int stateFlag(yapılacak işlem) paramatrelerini alır	*
 * *courseId : Dersin adı		stateFlag: yapılacak işlem	userID:kullanici*
 * kullanici dersin sahibi degilse USERCOURSE_ERR return edilir				*
 * işlemler makro ile belirtilmiştir										*
 * OPEN : Ders açma		CLOSE: dersi bitirme								*
 * AVAILABLE: ders ekleme çıkarmaya müsait									*
 * NAVAILABLE: ders ekleme çıkarmaya müsait değil							*
 * 																			*
 ***************************************************************************/
int processCourse(const char *courseID, int stateFlag, const char *userID)
{
	FILE *allFile, *ownFile;
	char filePath[NM_SN_SIZE];
	courses_t course, course2;
	
	sprintf(filePath, "Courses/%s.bin", courseID);
	allFile = fopen("Courses.bin", "rb+");
	ownFile = fopen(filePath, "rb+");
	
	if(ownFile == NULL)
	{
		fclose(allFile);
		return USER_COURSE_ERR;
	}
	
	fread(&course2, sizeof(courses_t), 1, ownFile);
		
	if(strcmp(course2.instructorID, userID))
	{	
		fclose(allFile);
		fclose(ownFile);
		return USER_COURSE_ERR;
	}
	
	do
	{
		fread(&course, sizeof(courses_t), 1, allFile);
		
		if(!strcmp(course.ID, courseID))
		{	
			switch(stateFlag)
			{
				case OPEN:
				
						course.isOpen = 1;
						course2.isOpen = 1;
						printf("%s is open anymore\n", courseID);
						break;
							
				case CLOSE:
							
						course.isOpen = 0;
						course2.isOpen = 0;
						course.isAvailable = 0;		
						course2.isAvailable = 0;
						printf("%s is close anymore\n", courseID);
						break;
				
				case AVAILABLE:
						
						course.isAvailable = 1;	
						course2.isAvailable = 1;
						printf("%s is available anymore\n", courseID);
						break;
						
				case NAVAILABLE:
						
						course.isAvailable = 0;		
						course2.isAvailable = 0;
						printf("%s is not available anymore\n", courseID);
						break;
			}
		}			
	}while(strcmp(course.ID, courseID));
	
	fseek(allFile, -sizeof(courses_t), SEEK_CUR);
	fwrite(&course, sizeof(courses_t), 1, allFile);
	rewind(ownFile);
	fwrite(&course2, sizeof(courses_t), 1, ownFile);
	

	fclose(allFile);
	fclose(ownFile);
	return 0;
}


 /***************************************************************************
 *																			*
 * 					Öğrenciler için olan menu								*
 *																			*
 ***************************************************************************/
void printMenu4Students(const char *name, const char *surname)
{
	printf("\n\t#################################################\n");
	printf("\t#\t\t\t\t\t\t#");
	printf("\n\t#\t\tACCESS SUCCESSFUL\t\t#\n");
	printf("\t#\t\t\t\t\t\t#\n");
	printf("\t#\t    WELCOME %s %s\t\t\t#\n", name, surname);
	printf("\t#\t\t\t\t\t\t#\n");
	printf("\t#################################################\n");
	printf("\t#\t\t\t\t\t\t#\n");
	printf("\t# \tMake a choice from following lines\t#\n");
	printf("\t#\t\t\t\t\t\t#\n");
	printf("\t# [1] >> ENROL COURSE\t\t\t\t#\n");
	printf("\t# [2] >> DISENROL COURSE\t\t\t#\n");
	printf("\t# [3] >> LIST THE COURSES YOU ENROLLED\t\t#\n");
	printf("\t# [4] >> CREATE TRANSCRIPT\t\t\t#\n");
	printf("\t# [5] >> EXIT\t\t\t\t\t#\n");
	printf("\t#\t\t\t\t\t\t#\n");
	printf("\t#################################################\n\n\n");
}


 /***************************************************************************
 *																			*
 * 					Eğtimciler için olan menu								*
 *																			*
 ***************************************************************************/
void printMenu4Instructors(const char *name, const char *surname)
{
	printf("\n\n\t#################################################\n");
	printf("\t#\t\t\t\t\t\t#");
	printf("\n\t#\t\tACCESS SUCCESSFUL\t\t#\n");
	printf("\t#\t\t\t\t\t\t#\n");
	printf("\t#\t    WELCOME %s %s\t\t#\n", name, surname);
	printf("\t#\t\t\t\t\t\t#\n");
	printf("\t#################################################\n");
	printf("\t# \tMake a choice from following lines\t#\n");
	printf("\t#\t\t\t\t\t\t#\n");
	printf("\t# [1] >> OPEN COURSE\t\t\t\t#\n");
	printf("\t# [2] >> FINALIZE COURSE\t\t\t#\n");
	printf("\t# [3] >> OPEN ADD-REMOVE MODE\t\t\t#\n");
	printf("\t# [4] >> CLOSE ADD-REMOVE MODE\t\t\t#\n");
	printf("\t# [5] >> ASSIGN GRADES\t\t\t\t#\n");
	printf("\t# [6] >> EDIT GRADES\t\t\t\t#\n");
	printf("\t# [7] >> LIST THE YOUR COURSES\t\t\t#\n");
	printf("\t# [8] >> LIST THE GRADES ANY COURSES\t\t#\n");
	printf("\t# [9] >> EXIT\t\t\t\t\t#\n");
	printf("\t#\t\t\t\t\t\t#\n");
	printf("\t#################################################\n\n\n");
}


/******************************************************************************/
/******************************************************************************/


/****************************************************************************
 *																			*
 * TUM FONKSIYONLAR KULLANICI İSLEMLERINI BELIRTIR							*
 * HEPSI O ANKI KULLANICININ ID SINI INPUT PARAMETRE OLARAK ALIR			*
 *																			*
 ***************************************************************************/
int studentChoice1(curr_user_t currentUser)
{
	char commandID[ID_SIZE];
	
	system("clear");
	printf("\nOPENED COURSES\n\n");
	if(listCourses4Add())
	{
		
		getIDCommand(commandID);
		
		if(checkAvailableOpen(commandID) == TRUE)
		{	
			if(!preqControl(currentUser.ID, commandID))
			{
				printf("You didn't pass preq course(s)\n");
				return 0;
			}
			if(enrolCourse(commandID, currentUser.ID))
			printf("You've already enrolled to this course\n\n");
		}
		else if(checkAvailableOpen(commandID) == FALSE)
			printf("\"%s\" is not avaialable for add\n\n", commandID);
		
		else
			printf("\"%s\" does not exist..\n\n", commandID);
	}
	return 0;
}



int studentChoice2(curr_user_t currentUser)
{	
	char commandID[ID_SIZE];
	int state;
	
	system("clear");
	
	printf("\nENROLLED COURSES\n\n");
	if(!listAndSaveEnrolledCourses(currentUser.ID, NO))
	{
		getIDCommand(commandID);
	
		if(checkAvailableOpen(commandID))
		{
			state = disenrolCourse(commandID, currentUser.ID, STUDENT);
			if(!state)
				printf("You disenrolled from %s\n\n", commandID);
			else if(state == USER_COURSE_ERR)
				printf("Not enrolled course\n\n");
		}
		else
			printf("You can not do this. Add Remove Mode is CLOSED\n");
	}
	
	return 0;
}




int instructorChoice1(curr_user_t currentUser)
{
	char commandID[ID_SIZE];
	
	system("clear");
	
	if(listCourses4Instructor(currentUser.ID, BEGIN, NO))
	{
		getIDCommand(commandID);
		if(processCourse(commandID, OPEN, currentUser.ID) == USER_COURSE_ERR)
			printf("The course is not yours or does not exist..\n");
	}
	
	return 0;
}



int instructorChoice2(curr_user_t currentUser)
{
	char commandID[ID_SIZE];
	int gradeState;
		
	system("clear");
	
	if(listCourses4Instructor(currentUser.ID, FINAL, NO))
	{
		getIDCommand(commandID);
		gradeState = isGraded(commandID, commandID, currentUser.ID, ALL);
		if(gradeState == USER_COURSE_ERR)
			printf("The course is not yours or does not exist..\n");
		
		else if(gradeState == FALSE)
			printf("All student not graded , all must be graded !!\n");
		
		else
		{	
			finalizeCourse(commandID, currentUser.ID);
		}
	}
	
	return 0;
}



int instructorChoice3(curr_user_t currentUser)
{
	char commandID[ID_SIZE];
	
	system("clear");
	if(listCourses4Instructor(currentUser.ID, AVAILABLE, NO))
	{	
		getIDCommand(commandID );
		
		if(processCourse(commandID, AVAILABLE, currentUser.ID) == USER_COURSE_ERR)
			printf("The course is not yours or does not exist..\n");
	}
	
	return 0;
}


int instructorChoice4(curr_user_t currentUser)
{
	char commandID[ID_SIZE];
	
	system("clear");
	
	if(listCourses4Instructor(currentUser.ID, NAVAILABLE, NO))
	{
		getIDCommand(commandID );
		if(processCourse(commandID, NAVAILABLE, currentUser.ID) == USER_COURSE_ERR)
			printf("The course is not yours or does not exist..\n");
	}
	
	return 0;
}



int instructorChoice5(curr_user_t currentUser)
{
	char commandID[ID_SIZE], commandID_2[ID_SIZE];
	double grade;
	
	system("clear");
	
	if(listCourses4Instructor(currentUser.ID, GRADE, NO))
	{
		printf("\nSelect a course for grade process mode..\n");
		getIDCommand(commandID);
		
		if(listStudents4Grade(commandID, currentUser.ID, NO) == USER_COURSE_ERR)
		{
			printf("Course does not exist or not yours..\n");
			return 0;
		}
								
		printf("\nSelect a student for grading..\n");
		getIDCommand(commandID_2);
		
		grade = getGrade();
		
		if(assignGrade(commandID_2, commandID, currentUser.ID, grade) == 			USER_COURSE_ERR)
		{
			printf("\nCourse or student does not exist or not yours..\n");		
		}
	}
	
	return 0;
}


int instructorChoice6(curr_user_t currentUser)
{
	char commandID[ID_SIZE], commandID_2[ID_SIZE];
	int gradeState;
	double grade;
	
	system("clear");
	if(listCourses4Instructor(currentUser.ID, GRADE, NO))
	{
		printf("\nSelect a course for edit process mode..\n");
		getIDCommand(commandID);
		
		
		if(listStudents4Grade(commandID, currentUser.ID, NO)== USER_COURSE_ERR)
		{
			printf("Course does not exist or not yours..\n");
			return 0;
		}
								
		printf("\nSelect a student for editing..\n");
		getIDCommand(commandID_2);
		gradeState = isGraded(commandID, commandID_2, currentUser.ID, ONE);
		if(gradeState == FALSE)
		{
			printf("You can not edit because it was not graded\n ");
			printf("Before make grade assignment\n");
			grade = getGrade();
			assignGrade(commandID_2, commandID, currentUser.ID, grade);
			return 0;
		}
		
		grade = getGrade();
		
		if(assignGrade(commandID_2, commandID, currentUser.ID, grade) == 			USER_COURSE_ERR)
		{
			printf("\nCourse or student does not exist or not yours..\n");		
		}
	}
	
	return 0;
}




int instructorChoice8(curr_user_t currentUser)
{
	char commandID[ID_SIZE];
	int state;
	
	system("clear");
	if(listCourses4Instructor(currentUser.ID, IF_OPEN, NO) == TRUE)
	{
		getIDCommand(commandID);
		state = listStudents4Grade(commandID, currentUser.ID, YES);
		if(state == 0)
		{
			printf("Any student was not enrol to the course\n");
		}
		if(state == USER_COURSE_ERR)
		{
			printf("Course does not exist or not yours..\n");
		}
	}
}