from flask import (
    Blueprint, flash, g, redirect, render_template, request, url_for
)
from werkzeug.exceptions import abort

from flaskr.auth import login_required
from flaskr.db import get_db
import sgx_call as sgx

bp = Blueprint('applet', __name__)

# @bp.route('/')
# def index():
#     db = get_db()
#     # posts = db.execute(
#     #     'SELECT p.id, title, body, created, author_id, username'
#     #     ' FROM post p JOIN user u ON p.author_id = u.id'
#     #     ' ORDER BY created DESC'
#     # ).fetchall()
#     posts = []
#     return render_template('applet/index.html', posts=posts)

@bp.route('/', methods=('GET', 'POST'))
@login_required
def index():
    if request.method == "POST":
        print(request.remote_addr)
        print(request.host)
        print(request.server)
        print(request.root_url)
        if request.environ.get('HTTP_X_FORWARDED_FOR') is None:
            print(request.environ['REMOTE_ADDR'])
        else:
            print(request.environ['HTTP_X_FORWARDED_FOR']) # if behind a proxy
        input_ = request.form['input']
        error = None
        if not input_:
            error = "Cannot work with empty input."
        
        if error is not None:
            flash(error)
        else:
            input_ = int(input_)
            
            output = 3*input_#sgx.csgx_mult3(input_)
        
            return render_template('applet/index.html', output=output)
    return render_template('applet/index.html', output=None)