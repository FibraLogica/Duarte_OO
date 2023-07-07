from flask import Flask, request, jsonify
import serial
from flask_sqlalchemy import SQLAlchemy
from datetime import datetime

# Cria uma app Flask
app = Flask(__name__)
app.config['SQLALCHEMY_DATABASE_URI'] = 'mysql://root@localhost/express'
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False
db = SQLAlchemy(app)


# Cria a class User para guardar dados sobre cada utilizador, como o id e o código do fingerprint.
class User(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    fingerprint_code = db.Column(db.String(120), unique=True, nullable=False)
    access_history = db.relationship('AccessHistory', backref='user', lazy=True)

# Declara um metodo "to_dict" onde retorna toda a informação sobre si.
    def to_dict(self):
        return {
            'id': self.id,
            'fingerprint_code': self.fingerprint_code,
            'access_history': [access.date.strftime('%Y-%m-%d %H:%M:%S') for access in self.access_history]
        }

<<<<<<< HEAD

=======
# Cria a classe AccessHistory, para poder registrar cada acesso que aconteceu.
>>>>>>> d157e21111f5b427a4f30302e8772aa4ab34a8a7
class AccessHistory(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    user_id = db.Column(db.Integer, db.ForeignKey('user.id'), nullable=False)
    date = db.Column(db.DateTime, nullable=False, default=datetime.utcnow)

    # Declara um metodo "to_dict" onde retorna toda a informação sobre si.
    def to_dict(self):
        return {
            'id': self.id,
            'user_id': self.user_id,
            'date': self.date.strftime('%Y-%m-%d %H:%M:%S')
        }

# Inicializa a comunicação serial com o Arduino
arduino = serial.Serial('COM5', 9600)
<<<<<<< HEAD

print("a")

=======
# Cria um sublink no website para criar uma conta
>>>>>>> d157e21111f5b427a4f30302e8772aa4ab34a8a7
@app.route('/create_user', methods=['POST'])
# Define a função create_user para criar utilizadores facilmente.
def create_user():
    # Recebe o json do request.
    data = request.get_json()
<<<<<<< HEAD
    print("a")
=======
    # Obtém o código de fingerprint.
>>>>>>> d157e21111f5b427a4f30302e8772aa4ab34a8a7
    fingerprint_code = data.get('fingerprint_code')
    # Cria um utilizador com o código.
    user = User(fingerprint_code=fingerprint_code)
    # Adiciona o utilizador á sessão.
    db.session.add(user)
    # Aplica mudanças.
    db.session.commit()
    # Retorna um print.
    return jsonify({'message': 'User created', 'user': user.to_dict()}), 201
# Cria um sublink no website para criar uma conta depois de o utilizador ter criado.
@app.route('/add_access/<user_id>', methods=['POST'])
def add_access(user_id):
    # Recebe o id do utilizador
    user = User.query.get(user_id)
    if not user:
        # Imprime que não existe tal utilizador
        return jsonify({'message': 'User not found'}), 404
    # Cria um historico de acesso do utilizador
    access = AccessHistory(user_id=user.id)
    # Acrescenta o historico á db
    db.session.add(access)
    # Aplica mudanças
    db.session.commit()
    # Retorna um print.
    return jsonify({'message': 'Access time added', 'user': user.to_dict()}), 200
# Cria um sublink no website para destrancar a porta.
@app.route('/unlock', methods=['POST'])
def unlock_door():
<<<<<<< HEAD

    # Cria um novo acesso no histórico
    user_id = request.json.get('user_id')
    user = User.query.get(user_id)
    
    if not user:
        return jsonify({'message': 'User not found'}), 404
    
    access = AccessHistory(user_id=user.id)
    db.session.add(access)
    db.session.commit()
    
    return jsonify({'message': 'Door unlocked and access time added', 'user': user.to_dict()}), 200

=======
    # É enviado um comando para destracar a porta no arduino.
    arduino.write(b'unlock')
    # Retorna um print.
    return jsonify({'message': 'Comando enviado'}), 200
# Inicia o servidor.
>>>>>>> d157e21111f5b427a4f30302e8772aa4ab34a8a7
if __name__ == '__main__':
    with app.app_context():
        db.create_all()
    app.run(host='0.0.0.0', debug=True)
