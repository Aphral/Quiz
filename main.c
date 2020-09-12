#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>


// Set the max size a question and answer can be.
const int MAX_QUIZ_LINE_SIZE = 2048;
const int NOT_ANSWERED = 0;
const int CORRECT_ANSWER = 1;
const int INCORRECT_ANSWER = 2;

// Make variable to keep count of the number of questions
int questionCount = 0;

// Make struct to store questions & answers along with an answered variable
// The answered variable will be set to either NOT_ANSWERED, CORRECT_ANSWER or INCORRECT_ANSWER
typedef struct QuestionAnswer {
    char *question;
    char *answer;
    int answered;
} questionAnswer;

// Get file address from user input
void get_filename(char *fileAddress) {
    printf("Please Enter File Address (Eg. qNa.txt): ");
    scanf("%s", fileAddress);
}

// Get difficulty level from user
int get_difficulty_level(int difficulty) {
    printf("\n******There are %d Questions In This Quiz******", questionCount);
    printf("\nPlease Enter The Level Of Difficulty 1-6: ");
    scanf("%d", &difficulty);
    // Check the level is valid
    while (difficulty > 6 || difficulty < 1) {
        printf("\nInvalid Option! ");
        printf("\nPlease Enter The Level Of Difficulty 1-6: ");
        scanf("%d", &difficulty);
    }
    // Return difficulty level chosen
    return difficulty;
}

// Removes spaces and newline characters
void remove_spaces(char *word) {
    int i;
    int j = 0;
    char *no_spaces = word;

    for (i = 0; i < strlen(word); i++, j++) {
        if (word[i] != ' ' && word[i] != '\n' && word[i] != '\r') {
            no_spaces[j] = word[i];
        } else {
            j--;
        }
    }
    no_spaces[j] = '\0';
}

// Make word uppercase
void make_uppercase(char *word) {
    for (int i = 0; i < strlen(word); i++) {
        word[i] = toupper(word[i]);
    }
}

// Read in questions and answers from file
questionAnswer *readQuestionsAndAnswers(const char *filepath) {
    // Declare Variables
    questionAnswer *qna;
    FILE *file;

    char *token1;
    char *token2;
    const char SPLIT[2] = "?";

    // Set buffer size to MAX_QUIZ_LINE_SIZE
    size_t buffer_size = MAX_QUIZ_LINE_SIZE;

    // Allocate memory to store line from file
    char *array = malloc(sizeof(char) * MAX_QUIZ_LINE_SIZE);

    // Open file, if file doesnt exist return NULL
    if ((file = fopen(filepath, "r")) == NULL) {
        return NULL;
    }

    // Get line from file
    while ((getline(&array, &buffer_size, file)) != -1) {
        if (questionCount == 0) {
            // If this is the first question allocate memory of the size of the struct
            qna = malloc(sizeof(struct QuestionAnswer));
        } else {
            // If this is not the first question reallocate memory to the question count multiplied by the size of the struct
            qna = realloc(qna, (questionCount + 1) * (sizeof(struct QuestionAnswer)));
        }

        // Split the array into question and answer by the question mark
        token1 = strtok(array, SPLIT);
        token2 = strtok(NULL, SPLIT);

        // Make the answer to the question upper case for comparision later
        // Remove spaces from answer
        make_uppercase(token2);
        remove_spaces(token2);

        // Allocate memory for question and answer based on the size of the Question and Answer read from file
        qna[questionCount].question = malloc(strlen(token1) + 1);
        qna[questionCount].answer = malloc(strlen(token2) + 1);

        // Set answered to NOT_ANSWERED as it has not been asked
        qna[questionCount].answered = NOT_ANSWERED;

        // Copy question and answer read from file into struct
        memcpy(qna[questionCount].question, token1, (strlen(token1) + 1));
        memcpy(qna[questionCount].answer, token2, (strlen(token2) + 1));

        // increment question counter
        questionCount++;
    }

    // Check For Errors
    int error = ferror(file);
    if (error != 0) {
        return NULL;
    }
    if(questionCount == 0){
        return NULL;
    }

    // Return array of structs pointer
    return qna;
}

int random_level() {
    // returns random number between 1 and 5
    int y = (rand() % 5) + 1;
    return y;
}

void print_clue(int difficulty, char *word) {
    // Declare variables to be used in function
    char shuffled_word[strlen(word) + 1];
    int i, j, temp;
    int x = 0;
    int y = 0;

    // make switch case depending on difficulty level
    switch (difficulty) {
        case 1:
            printf("?\n");
            break;
        case 2:
            for (int i = 0; i < strlen(word); i++) {
                putchar('-');
            }
            putchar('\n');
            break;
        case 3:
            putchar(word[0]);
            for (int i = 0; i < (strlen(word) - 2); i++) {
                putchar('-');
            }
            putchar(word[(strlen(word) - 1)]);
            putchar('\n');
            break;
        case 4:
            // while x and y are equal keep generating random numbers
            while (x == y) {
                x = (rand() % strlen(word));
                y = (rand() % strlen(word));
            }
            for (int j = 0; j < strlen(word); j++) {
                // if j is equal to x print the letter at position j in word
                if (j == x) {
                    putchar(word[x]);
                }
                // if j is equal to y print the letter at position j in word
                else if (j == y) {
                    putchar(word[y]);
                }
                // Otherwise print -
                else {
                    putchar('-');
                }
            }
            putchar('\n');
            break;
        case 5:
            // Make copy of word
            strcpy(shuffled_word, word);
            // Shuffle copy of word (Fisher Yates Shuffling Algorithm)
            for (i = strlen(word) - 1; i > 0; i--) {
                j = rand() % (i + 1);
                temp = shuffled_word[j];
                shuffled_word[j] = shuffled_word[i];
                shuffled_word[i] = temp;
            }
            // print the shuffled word
            printf("%s\n", shuffled_word);
            break;
    }
}

void ask_questions(questionAnswer *qnaPointer, int difficulty) {
    // Set the maximum user answer length to 65 (longest english word is 45 letters)
    const int MAX_SINGLE_WORD_ANSWER = 65;
    char userAnswer[MAX_SINGLE_WORD_ANSWER];
    int i = 0;
    while (i < questionCount) {
        // Generate random number between 1 and the number of questions
        int x = (rand() % questionCount);

        // Check if the question has already been asked
        if (qnaPointer[x].answered == NOT_ANSWERED) {
            // if not print the question and get users answer
            printf("\nQuestion: %s?\n", qnaPointer[x].question);
            print_clue(difficulty, qnaPointer[x].answer);
            printf("Please Enter Your Answer: ");
            scanf("%s", userAnswer);

            // make user input upper case and remove spaces and new line characters
            make_uppercase(userAnswer);
            remove_spaces(userAnswer);

            // if the users input matches the correct answer
            if (strcmp(userAnswer, qnaPointer[x].answer) == 0) {
                // print correct answer and set answered variable to CORRECT_ANSWER
                printf("Correct Answer\n");
                qnaPointer[x].answered = CORRECT_ANSWER;
            } else {
                // print wrong answer and set answered variable to INCORRECT_ANSWER
                printf("Wrong Answer\n");
                qnaPointer[x].answered = INCORRECT_ANSWER;
            }
            i++;
        }
    }
}

int get_correct_answers(questionAnswer *qnaPointer) {
    int correct = 0;

    // Count number of correct questions by going through structs and counting number
    // that have answered set to CORRECT_ANSWER
    for (int i = 0; i < questionCount; i++) {
        if (qnaPointer[i].answered == CORRECT_ANSWER) {
            correct++;
        }
    }
    return correct;
}

int write_to_file(const char *filepath, int difficulty, int num_correct) {
    // Write results of quiz to file
    FILE *file;
    // using "a" will create the file if it does not exist
    if ((file = fopen("quiz_history.txt", "a")) == NULL) {
        return -1;
    }
    fprintf(file, "File Name: %s Correct Answers: %d Total Questions: %d Diffuculty: %d\n", filepath, num_correct,
            questionCount, difficulty);
    fclose(file);
    return 1;
}


void print_correct_answers(questionAnswer *qnaPointer) {
    // If answered variable is INCORRECT_ANSWER
    // Print the question and the correct answer
    for (int i = 0; i < questionCount; i++) {
        if (qnaPointer[i].answered == INCORRECT_ANSWER) {
            printf("\nQuestion: %s?", qnaPointer[i].question);
            printf("\nAnswer: %s", qnaPointer[i].answer);
        }
    }
}

int main() {
    // Set a max address size
    const int MAX_ADDRESS_LEN = 512;

    // Declare variables
    questionAnswer *qnaPointer;
    int difficulty = 0;
    int num_correct;
    char view_results;
    view_results = 'y';
    int write_error = 0;
    char fileAddress [MAX_ADDRESS_LEN];

    // make rand truly random using time for use in other places
    srand(time(NULL));

    // get file name from user
    get_filename(fileAddress);

    // read questions from file
    qnaPointer = readQuestionsAndAnswers(fileAddress);
    // check for error
    if(qnaPointer == NULL){
        printf("Error Reading File!\n");
        exit(0);

    }

    // get level of difficulty
    difficulty = get_difficulty_level(difficulty);

    // if level of difficulty is 6 randomise a number between 1 and 5 to choose a level
    if (difficulty == 6) {
        difficulty = random_level();
    }

    // print questions depending on difficulty
    ask_questions(qnaPointer, difficulty);

    // get number of correct questions
    num_correct = get_correct_answers(qnaPointer);

    // write results to file
    write_error =  write_to_file(fileAddress, difficulty, num_correct);

    // check for error
    if (write_error == -1){
        printf("Error Writing To File!");
    }

    // Ask user if they want to see correct answers
    printf("\nWould You Like To View Correct Answers? (y/n): ");
    scanf(" %c", &view_results);

    // If user wants to see correct answers print questions and correct answers
    if (view_results == 'y' || view_results == 'Y') {
        print_correct_answers(qnaPointer);
    }

}
