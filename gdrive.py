import os
os.environ['OAUTHLIB_INSECURE_TRANSPORT'] = '1'
import pickle
import sys
import json
from google_auth_oauthlib.flow import InstalledAppFlow
from google.auth.transport.requests import Request
from googleapiclient.discovery import build
from googleapiclient.http import MediaFileUpload
from oauthlib.oauth2 import WebApplicationClient
from google_auth_oauthlib.flow import Flow

# If modifying these SCOPES, delete the file token.pickle.
SCOPES = ['https://www.googleapis.com/auth/drive.file']

def upload_file(filename, filepath):
    creds = None
    """Shows basic usage of the Drive v3 API.
    Uploads a file to Google Drive.
    """
    creds = None
    # The file token.pickle stores the user's access and refresh tokens, and is
    # created automatically when the authorization flow completes for the first
    # time.
    if os.path.exists('token.pickle'):
        with open('token.pickle', 'rb') as token:
            creds = pickle.load(token)
    # If there are no (valid) credentials available, let the user log in.
    if not creds or not creds.valid:
        if creds and creds.expired and creds.refresh_token:
            creds.refresh(Request())
        else:
            # Load client secrets
            with open('credentials.json', 'r') as secret:
                client_config = json.load(secret)

            os.environ['OAUTHLIB_INSECURE_TRANSPORT'] = '1'
            
            flow = InstalledAppFlow.from_client_config(
                client_config, scopes=SCOPES)
            
            creds = flow.run_local_server(port=8080)

            # Save the credentials for the next run
            with open('token.pickle', 'wb') as token:
                pickle.dump(creds, token)

    service = build('drive', 'v3', credentials=creds)

    # Call the Drive v3 API to upload the file
    file_metadata = {'name': filename}
    media = MediaFileUpload(filepath,
                            resumable=True)
    file = service.files().create(body=file_metadata, media_body=media, fields='id').execute()
    print(f"File ID: {file.get('id')}")
    return file.get('id')

def main():
    if len(sys.argv) < 3:
        print('Usage: python gdrive.py upload <filename> <filepath>')
        return

    command = sys.argv[1]

    if command == 'upload':
        filename = sys.argv[2]
        filepath = sys.argv[3]
        upload_file(filename, filepath)

if __name__ == '__main__':
    main()