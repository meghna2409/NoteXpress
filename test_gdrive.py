import unittest
from unittest.mock import patch, Mock
from googleapiclient.errors import HttpError
import httplib2

import gdrive  # your module

class TestGDrive(unittest.TestCase):
    @patch('gdrive.pickle.load')
    @patch('gdrive.build')
    @patch('gdrive.MediaFileUpload')
    def test_upload_file(self, mock_media_file_upload, mock_build, mock_pickle_load):
        # Mock the MediaFileUpload to not actually try to open a file
        mock_media_file_upload.return_value = Mock()

        # Mock the Google Drive API client
        mock_service = mock_build.return_value
        mock_service.files.return_value.create.return_value.execute.return_value = {'id': '123'}

        # Mock the credentials
        mock_creds = Mock()
        mock_creds.valid = True
        mock_pickle_load.return_value = mock_creds

        result = gdrive.upload_file('filename', 'filepath')

        self.assertEqual(result, '123')

    @patch('gdrive.pickle.load')
    @patch('gdrive.build')
    @patch('gdrive.MediaFileUpload')
    def test_upload_file_http_error(self, mock_media_file_upload, mock_build, mock_pickle_load):
        # Mock the MediaFileUpload to not actually try to open a file
        mock_media_file_upload.return_value = Mock()

        # Mock the Google Drive API client to raise an HttpError
        mock_service = mock_build.return_value
        resp = httplib2.Response({"status": 400})
        mock_service.files.return_value.create.return_value.execute.side_effect = HttpError(resp, b'content')

        # Mock the credentials
        mock_creds = Mock()
        mock_creds.valid = True
        mock_pickle_load.return_value = mock_creds

        with self.assertRaises(HttpError):
            gdrive.upload_file('filename', 'filepath')


if __name__ == '__main__':
    unittest.main()
