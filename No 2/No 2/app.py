from flask import Flask, Response
from flask_cors import CORS
from collections import OrderedDict
import json

app = Flask(__name__)
CORS(app)

@app.route('/data', methods=['GET'])
def get_data():
    data = OrderedDict([
        ("suhu_max", 36),
        ("suhu_min", 21),
        ("suhu_rate", 28.35),
        ("nilai_suhu_max_humid_max", [
            {
                "idx": 101,
                "suhu": 36,
                "humid": 36,
                "kecerahan": 25,
                "timestamp": "2010-09-18 07:23:48"
            },
            {
                "idx": 226,
                "suhu": 36,
                "humid": 36,
                "kecerahan": 27,
                "timestamp": "2011-05-02 12:29:34"
            }
        ]),
        ("month_year_max", [
            {"month_year": "9-2010"},
            {"month_year": "5-2011"}
        ])
    ])

    json_data = json.dumps(data, indent=4)

    return Response(json_data, mimetype='application/json')

if __name__ == '__main__':
    app.run(debug=True, port=5000)
