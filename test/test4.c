#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Function prototypes
void encryptMorse(char *message);
void convertToMorse(char *input, char *output);

int main() {
    // Take user input as an integer
    int userValue;
    printf("Enter an integer: ");
    scanf("%d", &userValue);

    // Open a file for reading
    FILE *file = fopen("input.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Redirect stdin to the file using freopen
    freopen("input.txt", "r", stdin);

    // Read the file and convert to Morse code
    char buffer[256];
    int cnt = 0;
    while (fgets(buffer, sizeof(buffer), stdin) != NULL  &&  cnt < userValue) {
        // Remove newline character if present
        buffer[strcspn(buffer, "\n")] = '\0';

        // Encrypt and convert to Morse code
        encryptMorse(buffer);
        char morseCode[1024] = "";  // Buffer to store Morse code
        convertToMorse(buffer, morseCode);
        printf("Encrypted Morse Code: %s\n", morseCode);

        // Run the loop till the userValue
        cnt++;
    }

    // Close the file
    fclose(file);

    return 0;
}

// Function to encrypt a message
void encryptMorse(char *message) {
    // This is a simple example, you can implement a more complex encryption algorithm
    for (int i = 0; i < strlen(message); i++) {
        message[i] += 1; // Add 1 to each character (simple encryption)
    }
}

// Function to convert a string to Morse code
void convertToMorse(char *input, char *output) {
    // Morse code conversion logic
    for (int i = 0; i < strlen(input); i++) {
        switch (toupper(input[i])) {
            case 'A':
                strcat(output, ".- ");
                break;
            case 'B':
                strcat(output, "-... ");
                break;
            case 'C':
                strcat(output, "-.-. ");
                break;
            case 'D':
                strcat(output, "-.. ");
                break;
            case 'E':
                strcat(output, ". ");
                break;
            case 'F':
                strcat(output, "..-. ");
                break;
            case 'G':
                strcat(output, "--. ");
                break;
            case 'H':
                strcat(output, ".... ");
                break;
            case 'I':
                strcat(output, ".. ");
                break;
            case 'J':
                strcat(output, ".--- ");
                break;
            case 'K':
                strcat(output, "-.- ");
                break;
            case 'L':
                strcat(output, ".-.. ");
                break;
            case 'M':
                strcat(output, "-- ");
                break;
            case 'N':
                strcat(output, "-. ");
                break;
            case 'O':
                strcat(output, "--- ");
                break;
            case 'P':
                strcat(output, ".--. ");
                break;
            case 'Q':
                strcat(output, "--.- ");
                break;
            case 'R':
                strcat(output, ".-. ");
                break;
            case 'S':
                strcat(output, "... ");
                break;
            case 'T':
                strcat(output, "- ");
                break;
            case 'U':
                strcat(output, "..- ");
                break;
            case 'V':
                strcat(output, "...- ");
                break;
            case 'W':
                strcat(output, ".-- ");
                break;
            case 'X':
                strcat(output, "-..- ");
                break;
            case 'Y':
                strcat(output, "-.-- ");
                break;
            case 'Z':
                strcat(output, "--.. ");
                break;
            case '0':
                strcat(output, "----- ");
                break;
            case '1':
                strcat(output, ".---- ");
                break;
            case '2':
                strcat(output, "..--- ");
                break;
            case '3':
                strcat(output, "...-- ");
                break;
            case '4':
                strcat(output, "....- ");
                break;
            case '5':
                strcat(output, "..... ");
                break;
            case '6':
                strcat(output, "-.... ");
                break;
            case '7':
                strcat(output, "--... ");
                break;
            case '8':
                strcat(output, "---.. ");
                break;
            case '9':
                strcat(output, "----. ");
                break;
            case ' ':
                strcat(output, " ");
                break;
            default:
                // Handle unsupported characters or ignore them
                break;
        }
    }
}