#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include<stdlib.h>
#include<conio.h>
#include<time.h>
#include<Windows.h>

#define HASH_SIZE 8 // defining our hashed password size

typedef struct rotor {
    char currentrotorposition;
    //the letter corresponding to which letter in default cipher does A get substituted into currently.
    //so the current position starts at ground setting and shifts one each time

    char groundsetting;
    //keyboard is ABCD...Z or BCDE...A or something else. 
    // So, ground setting = the letter that the substitution starts with. 
    //For example if default is "ABCDEF"->"WERTBN". Then the cipher is "ABCDEF"->"WERTBN" if A, "BCDEFG"->"WERTBN" if B, etc

    char ringsetting;
    //ABCD... go to the ABCD substitutions, or something else. 
    // For example if default is "ABCDEF"->"WERTBN". Then the cipher is "ABCDEF"->"WERTBN" if A, "ABCDEF"->"ERTBNG" if B, etc

    char notchposition;
    //the position of the rotor at which it will cause the next rotor to rotate with it

    int intoref[100];
    //the substitution cipher when signal goes from input to reflector

    int reftoout[100];
    //the substitution cipher when signal goes from reflector to output
} rot;



//all the parameters needed for a rotor

void maprotor(rot* rotornum, char cipher[], char rotoroffset, char ringsetting, char notchposition);
//maps the letters to their substitution ciphers

void shiftrotorby(rot* rotornum, int shiftby);
//rotates rotor by "shiftby" positions

char turntorange(char currentchar);
//turns input character to range of 0-25

char encryptedcharacter(char character, int pick1, int pick2, int pick3, char pickref);
//returns encrypted character to add to output file

void plugboard(char* privatekey, char* user_id);
//maps the plugboard 
//if A and E are connected via plugboard
//each A will become E, and each E will become A before going through rotor.
//opposite happens when coming out of the rotor for the encrypted character. 

char throughplugboard(char character);
//makes the character go through plugboard
//sees if the current character is connected to another in plugboard and replaces that. 

void red_colour();

void green_colour();

void reset_colour();

int hash_password(char* password, char* binary_hash);

void passwordinput(char* password);

void existing_user(char* user_id);

void create_public_key(char* public_key, char* private_key);

void to_private(char* public_key, char* private_key);

void create_key(char* user_id);

void initialiseBoard();

void drawBoard();

void aimoves();

int GameOver();

FILE* fileinput, * fileoutput, * filesettings;
char keyboard[] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27 };
rot r1, r2, r3, r4, r5, reflectorA, reflectorB; //initialise rotors and reflectors
char board[3][3];

int main(void) {
    char filename[100];
    char currentchar, encryptedchar; //input and output characters
    char rs1, rs2, rs3, rs4, rs5, rsA, rsB, gs1, gs2, gs3, gs4, gs5, gsA, gsB; //ring and ground settings
    char pickref, settingsname[50], settings[100];
    int pick1, pick2, pick3, n, i = 0;
    int user, flag = 0;
    char user_id[30];

    printf("\033[38;5;232m\033[48;5;220m\t\t\t\t\tWelcome to the 'Early age of Computers' platform\n");
    reset_colour();
    printf("\033[48;5;232m");
    printf("\t\t\t1. New user\n");     //press 1 for new user 
    printf("\t\t\t2. Existing user\n");//press 2 for existing user
    printf("\t\t\t3. Project Information\n\t\tType your Choice : ");
    scanf(" %d", &user);

    while (flag == 0) {
        if (user == 1)
        {
            flag = 1;
            printf("\t\t\tEnter user id: ");
            scanf(" %s", user_id);
            FILE* fptr = fopen("user_credentials.txt", "a+");
            //if file doesn't fptr stores null then we create that file here

            if (fptr == NULL)
            {
                fclose(fptr);
                fptr = fopen("user_credentials.txt", "w");
                fclose(fptr);
                fptr = fopen("user_credentials.txt", "r");
            }

            char buffer[30];
            //take input into buffer until pointer reaches end of file
            //after exiting from this loop we can surely say that we got the user id of the user which is not taken by any body else
            while (fscanf(fptr, "%s", buffer) != EOF)
            {
                if (strcmp(buffer, user_id) == 0) //if that user_id already exists
                {
                    red_colour();
                    printf("User ID already exists. Please choose another one.\n");
                    reset_colour();
                    char user_id2[30];
                    printf("Enter user id: ");
                    scanf(" %s", user_id2);
                    strcpy(user_id, user_id2);  //this step is performed to use the user_id variable every time 
                    fseek(fptr, 0, SEEK_SET);  //SEEK_SET is used to go to starting of file
                }
            }

            /*fclose(fptr);
            fptr = fopen("user_credentials.txt", "a");
            if (fptr == NULL) {
                printf("Didn't open");
                fptr = fopen("user_credentials.txt", "a");
            }*/

            printf("\t\t\tEnter password(max 20 characters): ");
            char password[25];
            passwordinput(password);

            char confirm[25];
            printf("\t\t\tConfirm password: ");
            scanf(" %s", confirm);

            while (strcmp(confirm, password) != 0) {
                red_colour();
                printf("\t\t\tPasswords do not match. Confirm password: ");
                reset_colour();
                scanf(" %s", confirm);
            }

            char binary_hash[HASH_SIZE + 1];       // storing hashed password in a 
            fprintf(fptr, "%s\n", user_id);
            hash_password(password, binary_hash); //function to hash the password

            fclose(fptr);
            char file_name[30];
            strcpy(file_name, user_id);
            strcat(file_name, ".txt");
            fptr = fopen(file_name, "w");       // since user is new ,no problem with using "w" instead of "a"

            fprintf(fptr, "%s\n", user_id);    // storing the user_id ,password in the file of user
            fprintf(fptr, "%s\n", binary_hash);

            system("cls"); //to clear the text printed in terminal 
            green_colour();
            printf("\t\t\t\tRegistration is completed succesfully.\n");
            reset_colour();

            fclose(fptr);
            create_key(user_id); //to create public key and store it in user's file
        }
        else if (user == 2)
        {
            flag = 1;
            printf("\t\t\tEnter user id: ");
            scanf(" %s", user_id);
            existing_user(user_id);
        }

        else if (user == 3) {
            flag = 0;
            Sleep(300);
            system("cls");
            printf("\033[48;5;210m\033[38;5;232m\t\t\t\t\tKnow more about our project!\n");
            reset_colour();
            printf("\033[48;5;232m");
            printf("1. Enigma Machine\n");
            printf("2. OXO Machine\n");
            printf("3. Go back\n\n");
            int flag2 = 0;
            while (flag2==0){
            printf("Enter your choice: ");
            int choice;
            scanf(" %d", &choice);
            switch (choice) {
            case 1:
                Sleep(300);
                system("cls");
                flag2 = 1;
                printf("\033[48;2;224;159;85m\033[38;2;34;34;34m\t\t\t\t\tTHE ENIGMA MACHINE\n\n");
                reset_colour();
                printf("\033[48;5;232m");
                Sleep(300);
                printf("The Enigma cipher machine was first patented by Scherbius in 1918. It was initially designed to be used by commercial companies to keep their communications secret. When Germany began rebuilding its military in the 1930s, the government took over the Enigmas and began using them for all of their secret communications. \n");
                Sleep(300);
                printf("Poland was aware that Germany would probably invade them first and built a cipher bureau to try to read enciphered German messages. The Poles were the first to determine how the Enigma machine worked and how to go about decoding its messages. When Poland was invaded, the Polish mathematicians were already helping the Allied forces develop strategies and machines which allowed them to read many important German messages during the war.\n");
                Sleep(300);
                printf("A team of codebreakers working at Bletchley Park in England and initially using the wiring data and a replica Enigma machine supplied by the Poles was able to decode most of the enigma-coded messages used by the German military even though the Germans changed the settings of the machine. The code name for the deciphering operation and the intelligence derived from it was 'Ultra'.\n");
                Sleep(300);
                printf("Each letter typed into the enigma machine's keyboard was converted to some other letter of the alphabet and displayed in a lighted window. Since the entire mechanism rotated each time a letter was entered, pressing the same letter three times could produce three different encodings. The encodings were produced by hard-wired code wheels and patch panels. The three code wheels could be mounted in a variety of positions and each one could be set to any letter of the alphabet. In addition, a patch panel on the front of the machine could be set up in many ways, making a vast number of combinations of cipher keys possible.\n");
                Sleep(300);
                printf("\nOur group has used the exact same method to rebuild Enigma in our program using historical schematics and circuit diagrams");
                Sleep(300);
                printf("\n\nThe Enigma Machine is an important historical precedent for computers, as this is what led \033[38;5;117m ALAN TURING \033[38;5;255m to create 'LE BOMBE' which is the first modern computer");
                printf("\n\nYou can know more from various places like 'The Enigma Museum', 'Wikipedia', 'Numberphile', 'Computerphile', and various other resources");
                Sleep(300);
                printf("\n\nGo Back? (Y) ");
                char yn;
                scanf(" %c", &yn);
                Sleep(300);
                system("cls");
                if (yn == 'Y'||yn=='y') {
                    break;
                }

            case 2:
                Sleep(300);
                system("cls");
                flag2 = 1;
                printf("\033[48;2;224;159;85m\033[38;2;34;34;34m\t\t\t\t\tTHE OXO MACHINE\n\n");
                reset_colour();
                printf("\033[48;5;232m");
                Sleep(300);
                printf("OXO is a video game developed by \033[38;5;117mA S Douglas\033[38;5;255m in 1952 which simulates a game of noughts and crosses (tic-tac-toe). It was one of the first games developed in the early history of video games. Douglas programmed the game as part of a thesis on human-computer interaction at the University of Cambridge.\n");
                Sleep(300);
                printf("The program was written for the Electronic Delay Storage Automatic Calculator (EDSAC). EDSAC was one of the first stored-program computers, with memory that could be read from or written to, and had three small cathode ray tube screens to display the state of the memory.\n");
                Sleep(300);
                printf("Douglas re-purposed one screen to demonstrate portraying other information to the user, such as the state of a noughts and crosses game. After the game served its purpose, it was discarded on the original hardware but later successfully reconstructed.\n");
                Sleep(300);
                printf("Each game was played by one user against an artificially intelligent opponent, which could play a 'perfect' game. The player entered their input using a rotary telephone controller, selecting which of the nine squares on the board they wished to move next. Their move would appear on the screen, and then the computer's move would follow; the game display only updated when the game state changed.\n");
                Sleep(300);
                printf("OXO was not available to the general public and could only be played in the University of Cambridge's Mathematical Laboratory by special permission, as the EDSAC could not be moved, and both the computer and the game were only intended for academic research purposes.\n");
                Sleep(300);
                printf("\nOur group has successfully implemented Douglas' OXO preserving some of the original design elements. This game marked a leap in Computers, with the introduction of games. This would prove to be just the beginning as both computers and games got much more sophisticated. \n");
                Sleep(300);
                printf("\n\nGo Back? (Y) ");
                scanf(" %c", &yn);
                Sleep(300);
                system("cls");
                if (yn == 'Y'|| yn=='y') {
                    break;
                }
            case 3:
                flag2 = 1;
                Sleep(300);
                system("cls");
                printf("\033[38;5;232m\033[48;5;220m\t\t\t\t\tWelcome to Early age of Computers platform\n");
                reset_colour();
                printf("\033[48;5;232m");
                printf("\t\t\t1. New user\n");     //press 1 for new user 
                printf("\t\t\t2. Existing user\n");//press 0 for existing user
                printf("\t\t\t3. Project Information\n\t\tType your Choice : ");
                scanf(" %d", &user);
                break;

            default:
                red_colour();
                printf("\nInvalid Choice.\n");
                reset_colour();
                break;
            }
            }
        }
        else
        {
            //here flag is still zero, so it goes through while loop again
            red_colour();
            printf("\t\tInvalid choice!\n");
            reset_colour();
            printf("\t\tType your Choice: ");
            scanf(" %d", &user);
        }
    }



    int encordec;
    flag = 0;
    while (flag == 0) {
        printf("Would you like to \n1. Encrypt a message to send using ENIGMA \n");
        printf("2. Decrypt a received message using ENIGMA \n");
        printf("3. Get an encrypted ENIGMA key to send\n");
        printf("4. Play OXO \n\n");
        printf("Your Choice : ");
        scanf(" %d", &encordec);
        Sleep(300);

        //switch case for encoding or decoding mode
        switch (encordec) {
        case 1:

            flag = 1;
            system("cls");
            printf("\033[38;2;34;34;34m\033[48;2;159;160;224m\t\t\t\tWelcome to Enigma Encryption. Based on the WW2 Device.\n");
            reset_colour();
            printf("\033[48;5;232m");
            printf("Enter input file name: ");
            scanf("%s", filename);

            fileinput = fopen(filename, "r");
            //asks for location of input file
            //opens input file in read mode

            if (fileinput == NULL) {
                red_colour();
                printf("Couldn't Open Input File. Please log in again.\n");
                reset_colour();
                return(-1);
            }


            printf("Enter ouput file name: ");
            scanf("%s", filename);
            fileoutput = fopen(filename, "w");
            //asks for location of output file
            //opens output file in write mode

            if (fileoutput == NULL) {
                red_colour();
                printf("Couldn't Open Output File. Please log in again.\n");
                reset_colour();
                fclose(fileinput);
                return(-1);
            }
            //checks if both files have opened

            strcpy(settingsname, user_id);
            strcat(settingsname, "_settings_");
            strcat(settingsname, filename);
            filesettings = fopen(settingsname, "w");
            //Creates a new file called initial settings and writes all initial settings onto that for later decryption

            printf("Enter Ground and Ring Settings for Rotor I: ");
            scanf(" %c %c", &gs1, &rs1);

            printf("Enter Ground and Ring Settings for Rotor II: ");
            scanf(" %c %c", &gs2, &rs2);

            printf("Enter Ground and Ring Settings for Rotor III: ");
            scanf(" %c %c", &gs3, &rs3);

            printf("Enter Ground and Ring Settings for Rotor IV: ");
            scanf(" %c %c", &gs4, &rs4);

            printf("Enter Ground and Ring Settings for Rotor V: ");
            scanf(" %c %c", &gs5, &rs5);

            printf("Enter Ground and Ring Settings for Reflector A: ");
            scanf(" %c %c", &gsA, &rsA);

            printf("Enter Ground and Ring Settings for Reflector B: ");
            scanf(" %c %c", &gsB, &rsB);


            //ask user for the initial settings for all rotors and reflectors.

            printf("Pick rotors [1-5]: ");
            scanf(" %d %d %d", &pick1, &pick2, &pick3);

            if (pick1 == pick2 || pick2 == pick3 || pick1 == pick3) {
                red_colour();
                printf("Cannot use same rotors on two positions. Input valid rotors: ");
                reset_colour();
                scanf(" %d %d %d", &pick1, &pick2, &pick3);
                printf("Pick reflectors [A-B]: ");
                scanf(" %c", &pickref);
                getchar();
            }
            else {
                printf("Pick reflectors [A-B]: ");
                scanf(" %c", &pickref);
            }
            Sleep(400);
            system("cls");
            green_colour();
            printf("Your Preferences have been recorded\n");
            reset_colour();
            printf("Do you want to send the file to anyone or store it?\n");
            printf("you can come back again to this page to get the key for the file\n");
            printf("1.Encrypt and send\n");
            printf("2.Encrypt and store\n\n");;
            printf("Your choice:");

            int choice;
            scanf("%d", &choice);
            int flag1 = 0;
            while (flag1 == 0) {
                switch (choice) {
                case 1:
                    flag1 = 1;
                    printf("Who do you want to be able to decrypt? (Enter their username) \n");
                    char receiver[50];
                    scanf("\n%s", receiver);
                    strcat(receiver, ".txt");
                    FILE* user_ptr = fopen(receiver, "r");

                    while (user_ptr == NULL) {

                        red_colour();
                        printf("User ID does not exist.");
                        reset_colour();

                        printf("Enter receiver username again: ");
                        scanf(" %s", receiver);
                        strcat(receiver, ".txt");
                        user_ptr = fopen(receiver, "r");

                    }

                    char privatekey[10];

                    for (int i = 1; i <= 2; ++i)
                    {
                        fscanf(user_ptr, "%s", privatekey);
                    }

                    fputc(gs1 + privatekey[0], filesettings);
                    fputc(rs1 + privatekey[1], filesettings);
                    fputc(gs2 + privatekey[2], filesettings);
                    fputc(rs2 + privatekey[3], filesettings);
                    fputc(gs3 + privatekey[4], filesettings);
                    fputc(rs3 + privatekey[5], filesettings);
                    fputc(gs4 + privatekey[6], filesettings);
                    fputc(rs4 + privatekey[7], filesettings);
                    fputc(gs5 + privatekey[0], filesettings);
                    fputc(rs5 + privatekey[1], filesettings);
                    fputc(gsA + privatekey[2], filesettings);
                    fputc(rsA + privatekey[3], filesettings);
                    fputc(gsB + privatekey[4], filesettings);
                    fputc(rsB + privatekey[5], filesettings);
                    putc(pick1 + 65 + privatekey[6], filesettings);
                    putc(pick2 + 65 + privatekey[7], filesettings);
                    putc(pick3 + 65 + privatekey[0], filesettings);
                    putc(pickref + privatekey[1], filesettings);

                    //pick the rotors and reflectors to be used for encryption

                    plugboard(privatekey, user_id);

                    for (i = 0;i < 8;i++) {
                        putc(privatekey[i], filesettings);
                    }

                    //set up letters to be connected through plugboard
                    green_colour();
                    printf("\t\t\tSuccessfully Encrypted!");
                    reset_colour();
                    fclose(user_ptr);
                    break;


                case 2:
                    flag1 = 1;
                    fputc(gs1, filesettings);
                    fputc(rs1, filesettings);
                    fputc(gs2, filesettings);
                    fputc(rs2, filesettings);
                    fputc(gs3, filesettings);
                    fputc(rs3, filesettings);
                    fputc(gs4, filesettings);
                    fputc(rs4, filesettings);
                    fputc(gs5, filesettings);
                    fputc(rs5, filesettings);
                    fputc(gsA, filesettings);
                    fputc(rsA, filesettings);
                    fputc(gsB, filesettings);
                    fputc(rsB, filesettings);
                    putc(pick1 + 65, filesettings);
                    putc(pick2 + 65, filesettings);
                    putc(pick3 + 65, filesettings);
                    putc(pickref, filesettings);

                    //pick the rotors and reflectors to be used for encryption

                    plugboard("\0\0\0\0\0\0\0\0", user_id);


                    //set up letters to be connected through plugboard
                    green_colour();
                    printf("\t\t\tSuccessfully Encrypted!");
                    reset_colour();
                    break;

                default:
                    printf("Enter a valid choice: ");
                    scanf(" %d", &choice);
                    break;
                }
            }

            break;


        case 2:

            flag = 1;
            system("cls");
            printf("\033[38;2;34;34;34m\033[48;2;159;160;224m\t\t\t\tWelcome to Enigma Decryption. Based on the WW2 Device.\n");
            reset_colour();
            printf("\033[48;5;232m");

            printf("Enter input file name: ");
            scanf("%s", filename);

            fileinput = fopen(filename, "r");
            //asks for location of input file
            //opens input file in read mode

            char inputfilename[50];
            strcpy(inputfilename, filename);

            if (fileinput == NULL) {
                red_colour();
                printf("Couldn't Open Input File. Please log in again.\n");
                reset_colour();
                return(-1);
            }


            printf("Enter ouput file name: ");
            scanf("%s", filename);
            fileoutput = fopen(filename, "w");
            //asks for location of output file
            //opens output file in write mode

            if (fileoutput == NULL) {
                red_colour();
                printf("Couldn't Open Output File. Please log in again.\n");
                reset_colour();
                fclose(fileinput);
                return(-1);
            }


            printf("Who sent you this message? (Enter username of sender) ");
            char sender_user_id[50];
            scanf(" %s", sender_user_id);
            strcpy(settingsname, sender_user_id);
            strcat(settingsname, "_settings_");
            strcat(settingsname, inputfilename);
            filesettings = fopen(settingsname, "r");

            while (filesettings == NULL) {
                red_colour();
                printf("\nThis user does not exist or has no file with the name %s\n", inputfilename);
                reset_colour();
                printf("Enter input file again: ");
                scanf(" %s", inputfilename);
                fileinput = fopen(inputfilename, "r");
                if (fileinput == NULL) {
                    red_colour();
                    printf("Couldn't Open Input File. Please log in again.\n");
                    reset_colour();
                    return(-1);
                }
                printf("Enter ouput file name again: ");
                scanf(" %s", filename);
                fileoutput = fopen(filename, "w");
                if (fileoutput == NULL) {
                    red_colour();
                    printf("Couldn't Open Output File. Please log in again.\n");
                    reset_colour();
                    fclose(fileinput);
                    return(-1);
                }
                printf("Who sent you this message? (Enter username of sender) ");
                scanf(" %s", sender_user_id);
                strcpy(settingsname, sender_user_id);
                strcat(settingsname, "_settings_");
                strcat(settingsname, inputfilename);
                filesettings = fopen(settingsname, "r");
            }

            while ((currentchar = fgetc(filesettings)) != EOF) {
                settings[i] = currentchar;
                i++;
            }
            //store all settings in a file, then put that in an array and assign values to the variables through that array
            char open[50];
            strcpy(open, user_id);
            strcat(open, ".txt");
            FILE* user_ptr;
            user_ptr = fopen(open, "r");
            char privatekey[10];
            for (int i = 1; i <= 2; ++i)
            {
                fscanf(user_ptr, "%s", privatekey);
            }

            gs1 = settings[0] - privatekey[0];
            rs1 = settings[1] - privatekey[1];
            gs2 = settings[2] - privatekey[2];
            rs2 = settings[3] - privatekey[3];
            gs3 = settings[4] - privatekey[4];
            rs3 = settings[5] - privatekey[5];
            gs4 = settings[6] - privatekey[6];
            rs4 = settings[7] - privatekey[7];
            gs5 = settings[8] - privatekey[0];
            rs5 = settings[9] - privatekey[1];
            gsA = settings[10] - privatekey[2];
            rsA = settings[11] - privatekey[3];
            gsB = settings[12] - privatekey[4];
            rsB = settings[13] - privatekey[5];

            //first 0-13 elements in the array are the rotor and reflector initial positions 

            pick1 = (int)settings[14] - 65 - privatekey[6];
            pick2 = (int)settings[15] - 65 - privatekey[7];
            pick3 = (int)settings[16] - 65 - privatekey[0];
            pickref = settings[17] - privatekey[1];

            //element 14,15,16,17 are the choices for the rotor

            n = (int)settings[18] - 65 - privatekey[0];

            //element 18 i.e. 'n' is the number of plugboard connections

            i = 19;
            int j = 1, k = 2, l;

            char checkprivate[10];
            for (int x = 0; x < 10;x++) {
                checkprivate[x] = '\0';
            }

            for (i = 19 + (2 * n), l = 0;i < 19 + 8 + (2 * n), l < 8; i++, l++) {
                checkprivate[l] = settings[i];
            }

            if (strcmp(checkprivate, privatekey) != 0) {
                red_colour();
                printf("You are not authorized to decrypt this message");
                reset_colour();
                exit(0);
            }

            while (i < (18 + (2 * n) + 1)) {
                if (k > 7) {
                    k = k - 8;
                }

                if (j > 7) {
                    j = j - 8;
                }

                keyboard[settings[i] - 65 - privatekey[j]] = settings[i + 1] - 65 - privatekey[k];
                keyboard[settings[i + 1] - 65 - privatekey[k]] = settings[i] - 65 - privatekey[j];
                i = i + 2;
                j = j + 2;
                k = k + 2;
            }

            //element 19->19+2n are the connected letters on the plugboard.
            //i.e. 19 and 20 are connected, 21 and 22 are connected, etc

            green_colour();
            printf("\t\t\tSuccessfully Decrypted!\n");
            reset_colour();
            break;


        case 3:
            system("cls");
            printf("\033[38;2;34;34;34m\033[48;2;159;160;224m\t\t\t\tWelcome to Enigma. Based on the WW2 Device.\n");
            reset_colour();
            printf("\033[48;5;232m");
            printf("\033[38;5;209m\t\t\tHere you can secure your previous Enigma key to send to another user.\n");
            reset_colour();
            printf("Choose your non-encrypted settings file: ");
            char unencsettings[50];
            scanf(" %s", unencsettings);
            filesettings = fopen(unencsettings, "r");
            if (filesettings == NULL) {
                red_colour();
                printf("Settings file does not exist\n");
                reset_colour();
                exit(-1);
            }

            printf("Who do you want to be able to decrypt? (Enter their username) \n");
            char receiver[50];
            scanf("\n%s", receiver);
            strcat(receiver, ".txt");
            user_ptr = fopen(receiver, "r");

            while (user_ptr == NULL) {

                red_colour();
                printf("User ID does not exist.");
                reset_colour();

                printf("Enter receiver username again: ");
                scanf(" %s", receiver);
                strcat(receiver, ".txt");
                user_ptr = fopen(receiver, "r");

            }

            while ((currentchar = fgetc(filesettings)) != EOF) {
                settings[i] = currentchar;
                i++;
            }

            gs1 = settings[0];
            rs1 = settings[1];
            gs2 = settings[2];
            rs2 = settings[3];
            gs3 = settings[4];
            rs3 = settings[5];
            gs4 = settings[6];
            rs4 = settings[7];
            gs5 = settings[8];
            rs5 = settings[9];
            gsA = settings[10];
            rsA = settings[11];
            gsB = settings[12];
            rsB = settings[13];

            //first 0-13 elements in the array are the rotor and reflector initial positions 

            pick1 = (int)settings[14] - 65;
            pick2 = (int)settings[15] - 65;
            pick3 = (int)settings[16] - 65;
            pickref = settings[17];

            //element 14,15,16,17 are the choices for the rotor

            n = (int)settings[18] - 65;
            //element 18 i.e. 'n' is the number of plugboard connections

            char plug[50];
            i = 19;
            j = 0;

            while (i < (18 + (2 * n) + 1)) {
                plug[j] = settings[i];
                i++;
                j++;
            }


            for (int i = 1; i <= 2; ++i)
            {
                fscanf(user_ptr, "%s", privatekey);
            }

            fclose(filesettings);

            filesettings = fopen(unencsettings, "w");

            fputc(gs1 + privatekey[0], filesettings);
            fputc(rs1 + privatekey[1], filesettings);
            fputc(gs2 + privatekey[2], filesettings);
            fputc(rs2 + privatekey[3], filesettings);
            fputc(gs3 + privatekey[4], filesettings);
            fputc(rs3 + privatekey[5], filesettings);
            fputc(gs4 + privatekey[6], filesettings);
            fputc(rs4 + privatekey[7], filesettings);
            fputc(gs5 + privatekey[0], filesettings);
            fputc(rs5 + privatekey[1], filesettings);
            fputc(gsA + privatekey[2], filesettings);
            fputc(rsA + privatekey[3], filesettings);
            fputc(gsB + privatekey[4], filesettings);
            fputc(rsB + privatekey[5], filesettings);
            putc(pick1 + 65 + privatekey[6], filesettings);
            putc(pick2 + 65 + privatekey[7], filesettings);
            putc(pick3 + 65 + privatekey[0], filesettings);
            putc(pickref + privatekey[1], filesettings);

            //pick the rotors and reflectors to be used for encryption

            fputc(n + 65 + privatekey[0], filesettings);

            k = 1;
            l = 2;

            for (j = 0; j < 2 * n;j = j + 2) {
                if (k > 7) {
                    k = k - 8;
                }
                if (l > 7) {
                    l = l - 8;
                }
                fputc(plug[j] + privatekey[k], filesettings);
                fputc(plug[j + 1] + privatekey[l], filesettings);
                k = k + 2;
                l = l + 2;
            }


            for (i = 0;i < 8;i++) {
                putc(privatekey[i], filesettings);
            }

            //set up letters to be connected through plugboard
            green_colour();
            printf("\t\t\tKey has been saved!");
            reset_colour();
            fclose(user_ptr);

            exit(0);
            break;

        case 4:
            system("cls");
            choice = 0;
            int player = 0;
            int winner = 0;
            int row = 0, column = 0;
            int friendorai;

            srand(time(NULL));

            //initialise game
            initialiseBoard();
            printf("\033[38;5;129m\t\t\t\tWelcome to OXO! Based on the 1952 game by A S Douglas\n");
            reset_colour();
            printf("1. Play against friend\n");
            printf("2. Play against AI\n");
            printf("Enter your choice: ");
            scanf(" %d", &friendorai);


            //Loops the Game till there are still spaces left in the board (i.e. gameover() is not 1 or 2)
            while (winner == 0) {
                system("cls");
                //clears system every turn

                printf("\n");
                drawBoard();
                printf("\n");

                // changes player turn each loop
                if (player == 0 || player == 2) {
                    player = 1;
                }

                else if (player == 1) {
                    player = 2;
                }

                if (friendorai == 2 && player == 2) {
                    aimoves();
                }

                else {
                    //do while is used to check if the choice input by the player already has something in it or not 
                    do {
                        printf("Player %d, enter a number between 1 and 9: ", player);
                        scanf("%d", &choice);

                        //works as choice = 5, then row = 1, column = 1. then we just do row = --choice/3 i.e. row = (int)4/3 = 1, and column = 4%3 = 1 
                        row = --choice / 3;
                        column = choice % 3;

                        if (choice < 0 || choice > 9 || board[row][column] != ' ') {
                            red_colour();
                            printf("Invalid move, try again.\n");
                            reset_colour();
                        }
                    } while (choice < 0 || choice > 9 || board[row][column] != ' ');

                    if (player == 1) {
                        board[row][column] = 'X';
                    }

                    else {
                        board[row][column] = 'O';
                    }
                }

                //checks winner, if winner = 0, then loop runs again, but if someone wins/draws, winner = 1, 2 respectively
                winner = GameOver();
            }
            system("cls");
            printf("\n");
            drawBoard();
            printf("\n");

            if (winner == 1) {
                if (friendorai == 1) {
                    printf("\t\t\t\aPlayer %d wins!\n", player); //prints the last player to move
                }
                else if (friendorai == 2) {
                    if (player == 1) {
                        printf("\t\t\t\t\aPlayer 1 wins!\n");
                    }
                    else {
                        printf("\t\t\t\t\aComputer wins!\n");
                    }
                }
            }

            else if (winner == 2) {
                printf("\t\t\t\t\aGame draw");
            }
            Sleep(2000);
            system("cls");
            break;


        default:
            printf("\n\n");
            red_colour();
            printf("Choose a valid choice\n");
            reset_colour();
            break;
        }
    }


    maprotor(&r1, "EKMF LGDQVZNTOWYHXUSPAI.BRCJ", gs1, rs1, 'Q');
    maprotor(&r2, "AJDKSIR.UXBLHWTMCQGZNPYF VOE", gs2, rs2, 'E');
    maprotor(&r3, "BDFHJLCP RTXVZNYEIWG.AKMUSQO", gs3, rs3, 'V');
    maprotor(&r4, "ESOV.PZJAYQUIRHXLNF TGKDCMWB", gs4, rs4, 'J');
    maprotor(&r5, "VZBRGITYUPSDNHLXAW.MJQOF ECK", gs5, rs5, 'Z');
    maprotor(&reflectorA, "EJM ALYXVBWFCRQUONTSPIKHG.DZ", gsA, rsA, 'A');
    maprotor(&reflectorB, "YRUHQ LDPXNGOKMIEB.ZCWVJATFS", gsB, rsB, 'A');
    //maps out all rotor connections 
    //here the rotors I-V and Reflectors A-B used are from wikipedia entry for actual rotors used in enigma. 

    while ((currentchar = fgetc(fileinput)) != EOF) {

        if (currentchar == 10) {
            fputc(10, fileoutput);
        }
        else {
            char character = turntorange(currentchar);
            //turns each character to preferred range of 0 to 25

            character = throughplugboard(character);
            //exchanges each character according to the input plugboard connections before going through rotors

            encryptedchar = encryptedcharacter(character, pick1, pick2, pick3, pickref);

            //encryptedchar = currentchar;
            fputc(encryptedchar, fileoutput);
        }
    }

    fclose(fileoutput);
    fclose(fileoutput);
    //close files at the end
}

void shiftrotorby(rot* rotornum, int shiftby) {
    int i, j;
    for (j = 0; j < shiftby; j++) {
        int temp1 = rotornum->intoref[27];
        for (i = 27; i > 0; i--) {
            rotornum->intoref[i] = rotornum->intoref[i - 1];
        }
        rotornum->intoref[0] = temp1;
    }

    //here temp1 becomes our last cipher element
    //and then each cipher element is shifted to the next in the array, and then our last cipher becomes our first element

    for (i = 0; i < 28; i++)
        rotornum->reftoout[rotornum->intoref[i]] = i;
    //then we do same to our reflected to output cipher. 

    /*printf("Shifted Rotor settings\n");
    for (char i = 0; i < 28; i++) {
        printf("In: %c\tO2C: %c\tC2O: %c\n\r", i + 65, rotornum->intoref[i] + 65, rotornum->reftoout[i] + 65);
        }*/

        //CHECK SHIFTED ROTORS

    if (rotornum->currentrotorposition + shiftby > 27) {
        rotornum->currentrotorposition = (rotornum->currentrotorposition + shiftby) - 28;
    }
    else if (rotornum->currentrotorposition + shiftby < 0) {
        rotornum->currentrotorposition = (rotornum->currentrotorposition + shiftby) + 28;
    }
    else rotornum->currentrotorposition = rotornum->currentrotorposition + shiftby;
    //this part sets the current position of the rotor. 
    // if it is more than 25+x, it is wrapped back to 0+x
}


void maprotor(rot* rotornum, char* cipher, char groundsetting, char ringsetting, char notchposition) {
    int i, initializegroundsetting = 0;
    for (i = 0; i < 28; i++) {
        if (cipher[i] >= 'A' && cipher[i] <= 'Z') {
            rotornum->intoref[i] = cipher[i] - 65;
        }
        else if (cipher[i] == ' ') {
            rotornum->intoref[i] = 26;
        }
        else if (cipher[i] == '.') {
            rotornum->intoref[i] = 27;
        }
    }
    //here cipher[i] is the string of cipher, where our intoref[1] = what A turns into, 2 is what B turns into etc. 
    // They are all shifted between 0-25 for ease of coding 

    for (i = 0; i < 28; i++) {
        rotornum->reftoout[rotornum->intoref[i]] = i;
    }
    //here reftoout[1] is the letter that turns to A in outtoref. 
    // for example, if A->F, and G->A. then intoref[A] = F, and reftoout[A]->G.  
    //this ensures that intoref is inverse of reftoout

    rotornum->currentrotorposition = 0;
    rotornum->notchposition = turntorange(notchposition);
    rotornum->groundsetting = turntorange(groundsetting);
    rotornum->ringsetting = turntorange(ringsetting);

    if (ringsetting - groundsetting < 0) {
        initializegroundsetting = ringsetting - groundsetting + 28;
    }
    else initializegroundsetting = ringsetting - groundsetting;

    if (initializegroundsetting != 0)
        shiftrotorby(rotornum, initializegroundsetting);
    rotornum->currentrotorposition = groundsetting - 65;
    //this part shifts your rotor according to the initial settings given

    /*printf("Rotor settings\n");
    for (char i = 0; i < 28; i++) {
        printf("In: %c\tO2C: %c\tC2O: %c\n\r", i + 65, rotornum->intoref[i] + 65, rotornum->reftoout[i] + 65);
    }*/

    //FOR CHECKING ROTOR SETTINGS
}

char turntorange(char currentchar) {
    char a;
    if (currentchar >= 'A' && currentchar <= 'Z')
        a = currentchar - 'A';
    else if (currentchar >= 'a' && currentchar <= 'z')
        a = currentchar - 'a';
    else if (currentchar == ' ')
        a = 26;
    else if (currentchar == '.')
        a = 27;
    return a;
}

void plugboard(char* privatekey, char* user_id) {
    int n, i = 0, j = 1, k = 2;
    char a, b, settingsname[50];
    printf("Enter number of desired plugboard connections (0-10): ");
    scanf("%d", &n);
    fputc(n + 65 + privatekey[0], filesettings);
    if (n > 0) {
        printf("Enter desired plugboard connections (Cannot connect 2 letters with same letter, or a letter with itself): \n");
        for (int i = 0; i < n; i++) {
            if (k > 7) {
                k = k - 8;
            }
            if (j > 7) {
                j = j - 8;
            }
            scanf("\n%c %c", &a, &b);
            fputc(a + privatekey[j], filesettings);
            fputc(b + privatekey[k], filesettings);
            keyboard[a - 65] = b - 65;
            keyboard[b - 65] = a - 65;
            //here, if A and E are switched then for our new keyboard, keyboard[A]=E, and keyboard[E]=A. 
            // i.e. keyboard[0]=4, and keyboard[4]=0. 
            j = j + 2;
            k = k + 2;
        }
    }
}

char throughplugboard(char character) {
    return keyboard[character];
}

char encryptedcharacter(char character, int pick1, int pick2, int pick3, char pickref) {
    int pf1, pf2, pf3, pr, ps1, ps2, ps3;
    char encrypt, nexttonotch3, nexttonotch2;
    //here order of rotors = [Reflector]<-[pick1]<-[pick2]<-[pick3] and then back again, output through pick3.
    rot* arr[] = { &r1, &r2, &r3, &r4, &r5, &reflectorA, &reflectorB };

    //first rotor shifts each time a new character is input
    shiftrotorby(arr[pick3 - 1], 1);

    if (arr[pick3 - 1]->notchposition + 1 > 27) {
        nexttonotch3 = 0;
    }
    else {
        nexttonotch3 = arr[pick3 - 1]->notchposition + 1;
    }
    //here we define a position as the one next to the notch position. 
    // here, if a rotor passes its notch position, it rotates with it the rotor after it
    //so if W is the notch position in a rotor then when its current position moves from W to X, it will rotate the next rotor too

    if (arr[pick3 - 1]->currentrotorposition == nexttonotch3) {
        shiftrotorby(arr[pick2 - 1], 1);
    }
    //defines shifting of pick 2 because of pick 3

    if (arr[pick2 - 1]->notchposition + 1 > 27) {
        nexttonotch2 = 0;
    }
    else {
        nexttonotch2 = arr[pick2 - 1]->notchposition + 1;
    }
    //wraps around next to notch position if notch position is Z

    if (arr[pick2 - 1]->currentrotorposition == nexttonotch2) {
        shiftrotorby(arr[pick2 - 1], 1);
        shiftrotorby(arr[pick1 - 1], 1);
    }
    //here, if pick 2 causes pick 1 to rotate, pick 2 needs to rotate too, and pick 3 rotates each time. 

    //printf("%d", character); //For checking progression of cipher
    pf3 = arr[pick3 - 1]->intoref[character];
    //going through pick 3 first time

    //printf("%d", pf3);
    pf2 = arr[pick2 - 1]->intoref[pf3];
    //going through pick 2 first time

    //printf("%d", pf2);
    pf1 = arr[pick1 - 1]->intoref[pf2];
    //going through pick 1 first time

    //printf("%d", pf1);
    pr = arr[(int)pickref - 60]->intoref[pf1];
    //gets reflected from chosen reflector

    //printf("%d", pr);
    ps1 = arr[pick1 - 1]->reftoout[pr];
    //going through pick 1 second time

    //printf("%d", ps1);
    ps2 = arr[pick2 - 1]->reftoout[ps1];
    //going through pick 2 second time

    //printf("%d", ps2);
    ps3 = arr[pick3 - 1]->reftoout[ps2];
    //going through pick 3 second time
    //this is our final output through rotors

    //printf("%d", ps3);

    encrypt = throughplugboard((char)ps3);
    //goes through plugboard again to replace anything that was replaced before

    //printf("%d", encrypt);

    if (encrypt >= 0 && encrypt <= 25)
        return encrypt + 65;

    else if (encrypt == 26)
        return ' ';

    else if (encrypt == 27)
        return '.';
}

void red_colour()
{
    printf("\033[0;31m");   //to change text to red colour \033 is escape sequence for colour changing and 31 is code for red colour
}

void green_colour()
{
    printf("\033[0;32m");  //to change colour to green
}

void reset_colour()
{
    printf("\033[0m");  //to reset colour 
}

// creating the hashed version of password and changing  into it's binary form
int hash_password(char* password, char* binary_hash) { //to hash and store the password

    long long int hashed_password; //hashing is just a way of encoding the password to store in data base
    hashed_password = 0;

    //to start the xor operation, so just initialise the hased_password variable with 0
    //as initially hased_password is 0 and if password is some "hello"
    //we will xor 0 with ascii value of h i.e the first letter(ascii value in binary form)
    //now xor this value of hashed_password with the ascii value of 2nd letter i.e 'e' and continue like this
    for (int i = 0; password[i] != '\0'; ++i)
    {
        (hashed_password) = (hashed_password) ^ password[i];
    }

    //it is just the copy of process we use to convert decimal into binary numbers
    // we are adding '0' here because the output of hashed_password % 2 is either 0 or 1 as each element of
    //string is character it would have stored the character having ascii value 0 or 1 so we are adding the ascii value
    //of 0 now LHS will be equal to 48 or 49(ascii value of 0 is 48) so now the character stored will be 1 or 0

    for (int i = 0; i < HASH_SIZE; ++i)
    {
        binary_hash[HASH_SIZE - 1 - i] = hashed_password % 2 + '0';
        hashed_password /= 2; //
    }

    binary_hash[HASH_SIZE] = '\0';
}

//this function is to take input of password from user while hiding it using * symbols
void passwordinput(char* password) {
    int i = 0;
    char ch;

    while (1) {
        ch = _getch(); //the getch() function holds the output to the command prompt, and only takes input
        if (ch == '\r' || ch == '\t') {
            password[i] = '\0';
            break;
        }

        else if (ch == '\b') {
            if (i > 0) {
                i--;
                printf("\b \b"); //if backspace is clicked, the character is decremented, and a \b removes a star 
            }
        }

        else {
            password[i++] = ch;
            printf("* \b"); //we print a star for every character we input
        }
    }
    printf("\n");
}


//the function to use when the user is existing
void existing_user(char* user_id) {
    char file_name[30];
    FILE* fptr = fopen("user_credentials.txt", "a+");
    char buffer[30];
    int count = 0;

    //to read the file in which all user ids are stored
    //and comparing the user id with the user ids stored in the user credentials.txt line by line
    while (count == 0)
    {
        fseek(fptr, 0, SEEK_SET);
        //to reset the file pointer to start so that we check the user name from the start of file again
       //SEEK_SET is used to set the pointer to start
       //0 refers to offset from start
       //fptr is used to refer to file : user_credentials

        while (fscanf(fptr, "%s", buffer) != EOF && count == 0)  // if user name is not found then continue till user enters the valid user id
        {
            if (strcmp(user_id, buffer) == 0)//if the user id is found then count is 1 to ensure that name is found and, break
            {
                count = 1;
                break;
            }
        }

        if (count == 0)
        {
            //if user_id was found then count would have been 1 if not,this is executed
            red_colour();
            printf("\t\t\tInvalid User ID\n\n");
            reset_colour();
            printf("\t\t\tEnter another User ID: ");
            scanf("%s", user_id);  //take the input from user again
            printf("\n");
        }


    }

    fclose(fptr);

    strcpy(file_name, user_id);
    strcat(file_name, ".txt");
    FILE* user_ptr = fopen(file_name, "r");

    char password[25];
    char ac_password[25];//actual password

    //to take input from second line
    // since one line contains only one word,so when we use fscanf two times it takes input from from second line
    for (int i = 1; i <= 2; ++i)
    {
        fscanf(user_ptr, "%s", ac_password);
    }
    char binary_hash[HASH_SIZE + 1]; // string to store the hashed password

    //count ==1 can be thought as "come into this only when user id is correct" and a form of infinite loop
    //until we break i.e until password gets correct
    while (count == 1)
    {
        printf("\t\t\tEnter the password: ");
        passwordinput(password);
        hash_password(password, binary_hash);

        //strcmp(ac_password, binary_hash) == 0 mean  the password user entered is same as the password stored
        if (strcmp(ac_password, binary_hash) == 0)
        {
            system("cls");
            green_colour();
            printf("\t\t\t\t\tWelcome back %s!\n", user_id);
            reset_colour();
            fclose(user_ptr);
            break;
        }
        else
        {
            red_colour();
            printf("Wrong password!\n");
            reset_colour();
        }
    }
}

// we have assumed the hased password stored is the private_key
//and this function creates the public_key from it 
//we are just xoring the previous element of private key with previous element of public key to get the 
//current element of public key and since we need to know the first element of public key so i just stored the 
//last element of private key in it
//since xor can be performed for only numbers i.e even if we do a^b(for an example) xor is operated between ascii value of a,b
//each time we are subtracting '0' to get value 1 or 0 in integer form since they are stored as '0' ,'1' characters 
//if we type cast by using (int) it gets converted into 48(ascii value of 0)

void create_public_key(char* public_key, char* private_key) {
    int size = strlen(private_key), i;

    public_key[0] = private_key[size - 1];

    for (i = 1; i < size; ++i)
    {
        public_key[i] = (public_key[i - 1] - '0') ^ (private_key[i - 1] - '0') + '0';
    }

    public_key[i] = '\0';//atlast making it a string
}


void to_private(char* public_key, char* private_key) {
    int size = strlen(public_key), i;

    for (i = 0; i < size - 1; ++i)
    {
        private_key[i] = (public_key[i] - '0') ^ (public_key[i + 1] - '0') + '0';
    }

    private_key[i] = public_key[0];
    private_key[i + 1] = '\0';
}

//this function is just used to create public key(indirectly) and store it in respective file of user.
//user's file name is user_id.txt so we have copied user id first and then ".txt".
//user_ptr pointer is closed and opened again because we want to switch from read mode to write(append here) mode
//fseek(user_ptr, 0, SEEK_END) is used to go to the end of file and store the public key in new line
//SEEK_END is the key word that is used to go to end of file
//as new line already created(we are doing \n every time we are printing in the file) just going to end of file is enough
void create_key(char* user_id) {
    char file_name[30];

    strcpy(file_name, user_id);
    strcat(file_name, ".txt");
    FILE* user_ptr = fopen(file_name, "r");

    char private_key[10];
    char public_key[10];

    for (int i = 1; i <= 2; ++i)
    {
        fscanf(user_ptr, "%s", private_key);
    }

    create_public_key(public_key, private_key);
    fclose(user_ptr);

    user_ptr = fopen(file_name, "a");
    fseek(user_ptr, 0, SEEK_END);
    fprintf(user_ptr, "%s\n", public_key);
    fclose(user_ptr);
}

//Initializes the tic tac toe board with all spaces as empty
void initialiseBoard() {
    int i, j;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            board[i][j] = ' ';
        }
    }
}

//Draws the tic tac toe board for each turn according to what's in the board[i][j]
void drawBoard() {
    int i, j, l = 1;
    green_colour();
    for (i = 0; i < 3; i++) {
        printf("\t\t\t\t\t %d :: %d :: %d", l, l + 1, l + 2);
        printf("\t | \t");
        printf(" %c :: %c :: %c ", board[i][0], board[i][1], board[i][2]);
        l = l + 3;
        if (i != 2) {
            printf("\n\t\t\t\t\t...::...::...");
            printf("\t | \t");
            printf("...::...::...\n");
        }
    }
    reset_colour();
    printf("\n");
}

void aimoves() {
    int choice;
    int row;
    int col;
    for (choice = 1; choice <= 9; choice++) {
        row = (choice - 1) / 3;
        col = (choice - 1) % 3;
        if (board[row][col] == ' ') {
            board[row][col] = 'O';
            if (GameOver() == 1) {
                return;
            }
            board[row][col] = ' ';
        }
    }

    for (choice = 1; choice <= 9; choice++) {
        row = (choice - 1) / 3;
        col = (choice - 1) % 3;
        if (board[row][col] == ' ') {
            board[row][col] = 'X';
            if (GameOver() == 1) {
                board[row][col] = 'O';
                return;
            }
            board[row][col] = ' ';
        }
    }

    while (1) {
        row = rand() % 3;
        col = rand() % 3;
        if (board[row][col] == ' ') {
            board[row][col] = 'O';
            return;
        }
    }

}

//Checks if game is over. Returns 1 if game has a winner, 2 if it is a draw, and 0 in other cases
int GameOver() {
    int i, j;
    // Check if either diagonal has all X or all O
    if ((board[0][0] == board[1][1] && board[0][0] == board[2][2] && (board[0][0] == 'X' || board[0][0] == 'O')) ||
        (board[0][2] == board[1][1] && board[0][2] == board[2][0] && (board[0][2] == 'X' || board[0][2] == 'O'))) {
        return 1;
    }
    else {
        // Check if each row / column has all X's or O's
        for (i = 0; i < 3; i++) {
            if ((board[i][0] == board[i][1] && board[i][0] == board[i][2] && (board[i][0] == 'X' || board[i][0] == 'O')) ||
                (board[0][i] == board[1][i] && board[0][i] == board[2][i] && (board[0][i] == 'X' || board[0][i] == 'O'))) {
                return 1;
            }
        }
    }

    // Check if no empty cells are left, if any cell is still empty it returns 0, if all cells are filled, it returns 2
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            if (board[i][j] == ' ') {
                return 0;
            }
        }
    }
    return 2;
}
