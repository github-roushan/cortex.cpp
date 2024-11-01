import pytest
import requests
from test_runner import start_server, stop_server

class TestApiModelImport:
    @pytest.fixture(autouse=True)
    def setup_and_teardown(self):
        # Setup
        success = start_server()
        if not success:
            raise Exception("Failed to start server")

        yield

        stop_server()

    @pytest.mark.skipif(True, reason="Expensive test. Only test when you have local gguf file.")
    def test_model_import_should_be_success(self):
        body_json = {'model': 'tinyllama:gguf',
                     'modelPath': '/path/to/local/gguf'}
        response = requests.post("http://localhost:3928/models/import", json=body_json)              
        assert response.status_code == 200

    @pytest.mark.skipif(True, reason="Expensive test. Only test when you have local gguf file.")
    def test_model_import_with_name_should_be_success(self):
        body_json = {'model': 'tinyllama:gguf',
                     'modelPath': '/path/to/local/gguf',
                     'name': 'test_model'}
        response = requests.post("http://localhost:3928/models/import", json=body_json)
        assert response.status_code == 200

    def test_model_import_with_invalid_path_should_fail(self):
        body_json = {'model': 'tinyllama:gguf',
                     'modelPath': '/invalid/path/to/gguf'}
        response = requests.post("http://localhost:3928/models/import", json=body_json)
        assert response.status_code == 400

    def test_model_import_with_missing_model_should_fail(self):
        body_json = {'modelPath': '/path/to/local/gguf'}
        response = requests.post("http://localhost:3928/models/import", json=body_json)
        print(response)
        assert response.status_code == 409