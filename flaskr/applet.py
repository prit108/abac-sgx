from flask import (
    Blueprint, flash, g, redirect, render_template, request, session, url_for
)
from werkzeug.exceptions import abort

from flaskr.auth import login_required
from flaskr.db import get_db
import json
import os
from ..mapper import build, evaluate_request
from poltree import *

bp = Blueprint('applet', __name__)
poltree_dict = {}
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
            output_ = 3*input_
            return render_template('applet/index.html', output=output)
    return render_template('applet/index.html', output=None)


@bp.route("/build", methods=('POST'))
@login_required 
def build_endpoint():
    if request.method == "POST":
        with request.files.items()[0] as value:
            if value.content_type == 'application/json':
                dst_filepath = os.path.join(os.getcwd()+"/received", value.filename)
                if not os.path.exists(os.path.dirname(dst_filepath)):
                    os.makedirs(os.path.dirname(dst_filepath))

                with open(dst_filepath, 'wb') as fd:
                    for chunk in value: 
                        fd.write(chunk)
                    print('Saved file as %s' % dst_filepath)
                
                global poltree_dict
                poltree_dict[session["user_id"]] = build(dst_filepath)           
        return render_template('applet/eval.html', output=None)

@bp.route("/evaluate", methods=('GET'))
@login_required 
def evaluate_endpoint():
    if request.method == "GET":
        ua = request.form['ua']
        oa = request.form['oa']
        access = request.form['access']
        
        if session["user_id"] not in poltree_dict:
            print("PolTree Not built.") 
            return 0
        else:
            return evaluate_request(poltree_dict[session["user_id"]], ua, oa, access)
