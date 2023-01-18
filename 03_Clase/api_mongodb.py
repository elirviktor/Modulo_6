from flask import Flask, request, Response
from flask_restful import Api, Resource
from pymongo import MongoClient
import json

# App Flask
app = Flask(__name__)
api = Api(app)

# Cliente MongoDB
user = "mongoUser"
password = "mongoPassword2022"
uri = f"mongodb+srv://{ user }:{ password }@clusterp3.5pr4guv.mongodb.net/?retryWrites=true&w=majority"
db_client = MongoClient(uri)

db = db_client.modulo6_db


class SensorList(Resource):
    collection = db.sensors

    def get(self):
        result = []
        query = self.collection.find()
        for doc in query:
            result.append(doc)
        return Response(json.dumps(result, default=str), mimetype="application/json")

    def post(self):
        request_body = request.get_json()
        self.collection.insert_one(request_body)

        return "OK", 201


api.add_resource(SensorList, "/sensors")


if __name__ == '__main__':
    app.run(debug=True, port=5001)