# Note Taking App with Google Drive Synchronization

The Note Taking App is a simple application for creating, editing, and deleting notes. It includes a synchronization feature that allows you to backup and sync your notes to Google Drive.

## Features

- Create, edit, and delete notes.
- Search for notes by keywords.
- Set reminders for notes.

## Requirements

- Node.js
- npm
- Express.js
- SQLite

## Installation

1. **Install Node.js and npm:**

Follow the instructions on the official [Node.js](https://nodejs.org/) website to install Node.js and npm.

2. **Install project dependencies:**

Navigate to your project directory in the terminal and run the following command:

```bash
npm install
```
This will install all necessary dependencies listed in package.json.

## Usage
1. **Start the server:**

Navigate to your project directory in the terminal and run the following command:

```bash
node server.js
```
The server will start on port 8000. You can access the Note Taking App at **http://localhost:8000**

2. **Use the application:**
The application exposes the following endpoints:

- **POST /create_note**: Creates a new note. The request body should contain JSON with title, content, and category of the note.

- **POST /edit_note**: Edits an existing note. The request body should contain JSON with id, title, content, and category of the note.

- **POST /delete_note**: Deletes an existing note. The request body should contain JSON with id of the note.

- **GET /search_notes**: Searches for notes containing a specified keyword. The query parameter keyword should be included in the URL.

- **POST /set_reminder**: Sets a reminder for a note. The request body should contain JSON with id of the note and reminder_time in milliseconds.

Please note that this is a very basic implementation and is not meant for production use. For a production application, consider using a more robust database and adding error handling and validation to the server.
