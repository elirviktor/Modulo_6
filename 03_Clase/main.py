from flask import Flask, request
from flask_restful import Api, Resource

app = Flask(__name__)
api = Api(app)

lista = {}
indice = 0


class SensorList(Resource):

    def get(self):
        return lista

    def post(self):
        global indice
        id = indice
        request_body = request.get_json()
        lista[id] = request_body["nombre"]
        indice += 1
        return {id: lista[id]}, 201


class Sensor(Resource):
    def get(self, id):
        return {id: lista[id]}, 200

    def put(self, id):
        #tarea para la casa para actualizar
        return

    def delete(self, id):
        del lista[id]
        return '', 204


api.add_resource(SensorList, "/sensors")
api.add_resource(Sensor, "/sensor/<int:id>")


if __name__ == '__main__':
    app.run(debug=True)