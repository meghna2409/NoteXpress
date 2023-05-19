#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

// Structure to hold notes and tasks
typedef struct Note {
    int id;
    char title[100];
    char content[500];
    char category[50];
    time_t created_at;
    time_t updated_at;
    struct Note *next;
} Note;

typedef struct Reminder {
    Note *note;
    time_t reminder_at;
} Reminder;

// Function prototypes
Note *create_note(char *title, char *content, char *category);
void edit_note(Note *note, char *title, char *content, char *category);
void delete_note(Note *note);
void search_notes(Note *head, char *keyword);
void *reminder_thread(void *arg);
void set_reminder(Note *note, time_t reminder_at);
void display_notes(Note *head);
void sync_with_gdrive(Note *note);

int main() {
    Note *head = NULL;

    // Add code to handle user input and call the relevant functions
    int choice;
    char title[100], content[500], category[50], keyword[100];
    int id;

    do {
        printf("Note-taking and Task-management Application\n");
        printf("1. Create a note\n");
        printf("2. Edit a note\n");
        printf("3. Delete a note\n");
        printf("4. Search notes\n");
        printf("5. Set a reminder\n");
        printf("6. Display all notes\n");
        printf("0. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();  // To consume the newline character after reading choice

        switch (choice) {
            case 1:
                printf("Enter the title: ");
                fgets(title, sizeof(title), stdin);
                title[strcspn(title, "\n")] = '\0';  // Remove the newline character
                printf("Enter the content: ");
                fgets(content, sizeof(content), stdin);
                content[strcspn(content, "\n")] = '\0';  // Remove the newline character
                printf("Enter the category: ");
                fgets(category, sizeof(category), stdin);
                category[strcspn(category, "\n")] = '\0';  // Remove the newline character

                Note *new_note = create_note(title, content, category);
                if (new_note) {
                    new_note->next = head;
                    head = new_note;
                }
                break;
            case 2:
                printf("Enter the note ID to edit: ");
                scanf("%d", &id);
                getchar();

                Note *note_to_edit = NULL;
                for (Note *current = head; current != NULL; current = current->next) {
                    if (current->id == id) {
                        note_to_edit = current;
                        break;
                    }
                }

                if (note_to_edit) {
                    printf("Enter the new title: ");
                    fgets(title, sizeof(title), stdin);
                    title[strcspn(title, "\n")] = '\0';  // Remove the newline character
                    printf("Enter the new content: ");
                    fgets(content, sizeof(content), stdin);
                    content[strcspn(content, "\n")] = '\0';  // Remove the newline character
                    printf("Enter the new category: ");
                    fgets(category, sizeof(category), stdin);
                    category[strcspn(category, "\n")] = '\0';  // Remove the newline character

                    edit_note(note_to_edit, title, content, category);
                } else {
                    printf("Note not found.\n");
                }
                break;
            case 3:
                printf("Enter the note ID to delete: ");
                scanf("%d", &id);
                getchar();

                Note *prev = NULL;
                Note *note_to_delete = head;
                while (note_to_delete != NULL) {
                    if (note_to_delete->id == id) {
                        break;
                    }
                    prev = note_to_delete;
                    note_to_delete = note_to_delete->next;
                }

                if (note_to_delete) {
                    if (prev == NULL) {
                        head = note_to_delete->next;
                    } else {
                        prev->next = note_to_delete->next;
                    }
                    delete_note(note_to_delete);
                    printf("Note with ID %d deleted.\n", id);
                } else {
                    printf("Note not found.\n");
                }
                break;
            case 4:
                printf("Enter the keyword to search: ");
                fgets(keyword, sizeof(keyword), stdin);
                keyword[strcspn(keyword, "\n")] = '\0';  // Remove the newline character

                search_notes(head, keyword);
                break;
            case 5:
                printf("Enter the note ID to set a reminder for: ");
                scanf("%d", &id);
                getchar();

                Note *note_to_remind = NULL;
                for (Note *current = head; current != NULL; current = current->next) {
                    if (current->id == id) {
                        note_to_remind = current;
                        break;
                    }
                }

                if (note_to_remind) {
                    time_t reminder_time;
                    printf("Enter the reminder time in seconds from now: ");
                    scanf("%ld", &reminder_time);
                    getchar();

                    set_reminder(note_to_remind, time(NULL) + reminder_time);
                } else {
                    printf("Note not found.\n");
                }
                break;
            case 6:
                display_notes(head);
                break;
            case 0:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice.\n");
        }
    } while (choice != 0);

    // Free allocated memory for notes before exiting
    while (head != NULL) {
        Note *temp = head;
        head = head->next;
        delete_note(temp);
    }

    return 0;
}

Note *create_note(char *title, char *content, char *category) {
    Note *new_note = (Note *) malloc(sizeof(Note));
    if (!new_note) {
        printf("Error allocating memory for new note.\n");
        return NULL;
    }

    new_note->id = rand() % 1000000;
    strcpy(new_note->title, title);
    strcpy(new_note->content, content);
    strcpy(new_note->category, category);
    new_note->created_at = time(NULL);
    new_note->updated_at = time(NULL);
    new_note->next = NULL;

    // Create a text file with the note's details
    char file_path[100];
    sprintf(file_path, "note_%d.txt", new_note->id);
    FILE *file = fopen(file_path, "w");
    if (file) {
        fprintf(file, "Title: %s\n", title);
        fprintf(file, "Content: %s\n", content);
        fprintf(file, "Category: %s\n", category);
        fprintf(file, "Created at: %s", ctime(&new_note->created_at));
        fprintf(file, "Updated at: %s", ctime(&new_note->updated_at));
        fclose(file);

        // Sync with Google Drive
        sync_with_gdrive(new_note);
    } else {
        printf("Error creating file for note.\n");
    }

    return new_note;
}

void edit_note(Note *note, char *title, char *content, char *category) {
    strcpy(note->title, title);
    strcpy(note->content, content);
    strcpy(note->category, category);
    note->updated_at = time(NULL);

    // Update the text file with the note's details
    char file_path[100];
    sprintf(file_path, "note_%d.txt", note->id);
    FILE *file = fopen(file_path, "w");
    if (file) {
        fprintf(file, "Title: %s\n", title);
        fprintf(file, "Content: %s\n", content);
        fprintf(file, "Category: %s\n", category);
        fprintf(file, "Created at: %s", ctime(&note->created_at));
        fprintf(file, "Updated at: %s", ctime(&note->updated_at));
        fclose(file);
        sync_with_gdrive(note);
    } else {
        printf("Error updating file for note.\n");
    }
}

void delete_note(Note *note) {
    // Delete the text file for the note
    char file_path[100];
    sprintf(file_path, "note_%d.txt", note->id);
    if (remove(file_path) != 0) {
        printf("Error deleting file for note.\n");
    }
    free(note);
}

void search_notes(Note *head, char *keyword) {
    Note *current = head;
    int count = 0;

    printf("Notes containing keyword \"%s\":\n", keyword);

    while (current != NULL) {
        if (strstr(current->title, keyword) || strstr(current->content, keyword) || strstr(current->category, keyword)) {
            count++;
            printf("\nNote ID: %d\n", current->id);
            printf("Title: %s\n", current->title);
            printf("Content: %s\n", current->content);
            printf("Category: %s\n", current->category);
            printf("Created at: %s", ctime(&current->created_at));
            printf("Updated at: %s\n", ctime(&current->updated_at));
        }
        current = current->next;
    }

    if (count == 0) {
        printf("No notes found containing the keyword \"%s\".\n", keyword);
    }
}

void *reminder_thread(void *arg) {
    Reminder *reminder = (Reminder *)arg;
    time_t current_time;

    while (1) {
        current_time = time(NULL);
        if (current_time >= reminder->reminder_at) {
            printf("\n[REMINDER] Note ID: %d, Title: %s\n", reminder->note->id, reminder->note->title);
            free(reminder);
            break;
        }
        sleep(1);
    }

    pthread_exit(NULL); 
}


void set_reminder(Note *note, time_t reminder_at) {
    Reminder *reminder = (Reminder *)malloc(sizeof(Reminder));
    reminder->note = note;
    reminder->reminder_at = reminder_at;

    pthread_t reminder_tid;
    pthread_create(&reminder_tid, NULL, reminder_thread, (void *)reminder);
    pthread_detach(reminder_tid);
}

void display_notes(Note *head) {
    Note *current = head;

    if (current == NULL) {
        printf("No notes found.\n");
        return;
    }

    printf("All notes:\n");

    while (current != NULL) {
        printf("\nNote ID: %d\n", current->id);
        printf("Title: %s\n", current->title);
        printf("Content: %s\n", current->content);
        printf("Category: %s\n", current->category);
        printf("Created at: %s", ctime(&current->created_at));
        printf("Updated at: %s\n", ctime(&current->updated_at));
        current = current->next;
    }
}

void sync_with_gdrive(Note *note) {
    char file_path[500];
    sprintf(file_path, "note_%d.txt", note->id);
    char command[500];
    sprintf(command, "python3 gdrive.py upload %.200s %.200s", file_path, file_path);
    system(command);
}