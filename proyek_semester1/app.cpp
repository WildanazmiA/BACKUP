// app.cpp
// Single-file minimal productivity tracker
// Dependencies: httplib.h (single-header) and sqlite3

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <sstream>
#include <vector>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include "nlohmann/json.hpp" // optional: if not available, simple manual JSON used below
#include "httplib.h"
#include "sqlite3.h"

using namespace std;
using namespace httplib;

// --- simple helpers ---
string now_iso() {
    time_t t = time(nullptr);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", gmtime(&t));
    return string(buf);
}

static int exec_sql(sqlite3* db, const string& sql) {
    char* err = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &err);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << (err ? err : "") << endl;
        sqlite3_free(err);
    }
    return rc;
}

// --- embedded frontend files ---
const char* INDEX_HTML = R"HTML(
<!doctype html>
<html>
<head>
<meta charset="utf-8">
<title>Productivity Tracker</title>
<meta name="viewport" content="width=device-width,initial-scale=1">
<link rel="stylesheet" href="/style.css">
</head>
<body>
<div id="app">
  <header>
    <h1>Productivity Tracker</h1>
  </header>

  <section id="todo">
    <h2>To Do</h2>
    <input id="newTask" placeholder="Tambahkan tugas"/>
    <button id="addBtn">Tambah</button>
    <ul id="tasks"></ul>
  </section>

  <section id="pomodoro">
    <h2>Pomodoro</h2>
    <div id="pomInfo">Status: idle</div>
    <button id="startPom">Mulai 25m</button>
    <button id="stopPom">Berhenti</button>
  </section>

  <section id="records">
    <h2>Track Record</h2>
    <ul id="recordsList"></ul>
  </section>

  <section id="dashboard">
    <h2>Dashboard</h2>
    <div id="dashSummary"></div>
  </section>

  <section id="suggestions">
    <h2>Saran Kegiatan</h2>
    <ul id="sugs"></ul>
  </section>

</div>
<script src="/app.js"></script>
</body>
</html>
)HTML";

const char* STYLE_CSS = R"CSS(
body { font-family: Arial, sans-serif; margin: 12px; }
header h1 { margin: 0 0 12px 0; }
section { border: 1px solid #ddd; padding: 10px; margin-bottom: 12px; border-radius: 6px; }
button { margin-left: 6px; }
ul { padding-left: 18px; }
.completed { text-decoration: line-through; color: gray; }
)CSS";

const char* APP_JS = R"JS(
// Minimal frontend. No frameworks.
function q(id){ return document.getElementById(id); }

async function fetchJSON(url, opts) {
  const res = await fetch(url, opts);
  return res.json();
}

async function loadTasks() {
  const data = await fetchJSON('/api/tasks');
  const ul = q('tasks');
  ul.innerHTML = '';
  data.forEach(t => {
    const li = document.createElement('li');
    li.textContent = t.title + ' (' + t.created_at.split('T')[0] + ')';
    if (t.done) li.classList.add('completed');
    const toggle = document.createElement('button');
    toggle.textContent = t.done ? 'Batal' : 'Selesai';
    toggle.onclick = async () => {
      await fetch('/api/tasks/' + t.id, { method:'PUT', headers:{'Content-Type':'application/json'}, body: JSON.stringify({done: !t.done})});
      loadTasks();
      loadDashboard();
    };
    const del = document.createElement('button');
    del.textContent = 'Hapus';
    del.onclick = async () => {
      await fetch('/api/tasks/' + t.id, { method:'DELETE' });
      loadTasks();
      loadDashboard();
    };
    li.appendChild(toggle);
    li.appendChild(del);
    ul.appendChild(li);
  });
}

async function addTask() {
  const v = q('newTask').value.trim();
  if (!v) return;
  await fetch('/api/tasks', { method:'POST', headers:{'Content-Type':'application/json'}, body: JSON.stringify({title:v})});
  q('newTask').value = '';
  loadTasks();
  loadDashboard();
}

async function startPomodoro() {
  await fetch('/api/pomodoro/start', { method:'POST', headers:{'Content-Type':'application/json'}, body: JSON.stringify({duration:25})});
  q('pomInfo').textContent = 'Status: running';
  loadRecords();
  loadDashboard();
}

async function stopPomodoro() {
  await fetch('/api/pomodoro/stop', { method:'POST' });
  q('pomInfo').textContent = 'Status: idle';
  loadRecords();
  loadDashboard();
}

async function loadRecords() {
  const data = await fetchJSON('/api/records');
  const ul = q('recordsList');
  ul.innerHTML = '';
  data.forEach(r => {
    const li = document.createElement('li');
    li.textContent = r.type + ' - ' + r.note + ' - ' + r.at;
    ul.appendChild(li);
  });
}

async function loadDashboard() {
  const d = await fetchJSON('/api/dashboard');
  q('dashSummary').textContent = 'Tugas selesai hari ini: ' + d.tasks_done_today + ', Sesi Pomodoro hari ini: ' + d.pomos_today;
  loadSuggestions();
}

async function loadSuggestions() {
  const s = await fetchJSON('/api/suggestions');
  const ul = q('sugs');
  ul.innerHTML = '';
  s.forEach(x => {
    const li = document.createElement('li');
    li.textContent = x;
    ul.appendChild(li);
  });
}

document.addEventListener('DOMContentLoaded', () => {
  q('addBtn').onclick = addTask;
  q('startPom').onclick = startPomodoro;
  q('stopPom').onclick = stopPomodoro;
  loadTasks();
  loadRecords();
  loadDashboard();
});
)JS";

// --- Database helpers ---
struct DB {
    sqlite3* db = nullptr;
    DB(const string& path) {
        if (sqlite3_open(path.c_str(), &db) != SQLITE_OK) {
            cerr << "Cannot open DB\n";
            db = nullptr;
        } else {
            init();
        }
    }
    ~DB() {
        if (db) sqlite3_close(db);
    }
    void init() {
        exec_sql(db, "PRAGMA foreign_keys = ON;");
        exec_sql(db,
            "CREATE TABLE IF NOT EXISTS tasks ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "title TEXT NOT NULL,"
            "done INTEGER DEFAULT 0,"
            "created_at TEXT NOT NULL);");
        exec_sql(db,
            "CREATE TABLE IF NOT EXISTS pomodoros ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "started_at TEXT,"
            "stopped_at TEXT,"
            "duration_min INTEGER DEFAULT 0);");
        exec_sql(db,
            "CREATE TABLE IF NOT EXISTS records ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "type TEXT,"
            "note TEXT,"
            "at TEXT);");
    }

    vector<map<string,string>> query_rows(const string& q) {
        vector<map<string,string>> rows;
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, q.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return rows;
        int cols = sqlite3_column_count(stmt);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            map<string,string> row;
            for (int i=0;i<cols;i++) {
                const char* name = sqlite3_column_name(stmt, i);
                const unsigned char* val = sqlite3_column_text(stmt, i);
                row[name] = val ? string((const char*)val) : string();
            }
            rows.push_back(row);
        }
        sqlite3_finalize(stmt);
        return rows;
    }

    bool exec_stmt(const string& sql) {
        return exec_sql(db, sql) == SQLITE_OK;
    }
};

// --- Global DB and pomodoro simple state ---
DB gdb("prodtracker.db");
bool pom_running = false;
time_t pom_start = 0;
int pom_duration = 0; // minutes

// --- main ---
int main() {
    Server svr;

    // serve frontend
    svr.Get("/", [](const Request&, Response& res){
        res.set_content(INDEX_HTML, "text/html");
    });
    svr.Get("/style.css", [](const Request&, Response& res){
        res.set_content(STYLE_CSS, "text/css");
    });
    svr.Get("/app.js", [](const Request&, Response& res){
        res.set_content(APP_JS, "application/javascript");
    });

    // API: tasks
    svr.Get("/api/tasks", [](const Request&, Response& res){
        auto rows = gdb.query_rows("SELECT id,title,done,created_at FROM tasks ORDER BY id DESC;");
        // build json manually
        string out = "[";
        bool first = true;
        for (auto &r: rows) {
            if (!first) out += ",";
            first = false;
            out += "{";
            out += "\"id\":" + r["id"] + ",";
            out += "\"title\":\"" + string(r["title"]) + "\",";
            out += "\"done\":" + r["done"] + ",";
            out += "\"created_at\":\"" + r["created_at"] + "\"";
            out += "}";
        }
        out += "]";
        res.set_content(out, "application/json");
    });

    svr.Post("/api/tasks", [](const Request& req, Response& res){
        // simple parse body for title. Expect JSON {title:"..."}
        string title = "untitled";
        auto pos = req.body.find("\"title\"");
        if (pos != string::npos) {
            auto p2 = req.body.find(":", pos);
            if (p2 != string::npos) {
                auto start = req.body.find_first_of("\"", p2+1);
                auto end = req.body.find_first_of("\"", start+1);
                if (start!=string::npos && end!=string::npos) {
                    title = req.body.substr(start+1, end-start-1);
                }
            }
        }
        string t = now_iso();
        string sql = "INSERT INTO tasks(title,created_at) VALUES('" + title + "','" + t + "');";
        gdb.exec_stmt(sql);
        // also record
        string rec = "INSERT INTO records(type,note,at) VALUES('task','added " + title + "','" + t + "');";
        gdb.exec_stmt(rec);
        res.set_content("{\"ok\":true}", "application/json");
    });

    svr.Put(R"(/api/tasks/(\d+))", [](const Request& req, Response& res){
        int id = stoi(req.matches[1]);
        // parse done boolean
        bool done = false;
        if (req.body.find("\"done\"") != string::npos) {
            if (req.body.find("true") != string::npos) done = true;
        }
        string sql = "UPDATE tasks SET done = " + string(done ? "1" : "0") + " WHERE id = " + to_string(id) + ";";
        gdb.exec_stmt(sql);
        string t = now_iso();
        string note = string(done ? "completed task id " : "uncompleted task id ") + to_string(id);
        string rec = "INSERT INTO records(type,note,at) VALUES('task','" + note + "','" + t + "');";
        gdb.exec_stmt(rec);
        res.set_content("{\"ok\":true}", "application/json");
    });

    svr.Delete(R"(/api/tasks/(\d+))", [](const Request& req, Response& res){
        int id = stoi(req.matches[1]);
        string sql = "DELETE FROM tasks WHERE id = " + to_string(id) + ";";
        gdb.exec_stmt(sql);
        string t = now_iso();
        string rec = "INSERT INTO records(type,note,at) VALUES('task','deleted task id " + to_string(id) + "','" + t + "');";
        gdb.exec_stmt(rec);
        res.set_content("{\"ok\":true}", "application/json");
    });

    // Pomodoro start
    svr.Post("/api/pomodoro/start", [](const Request& req, Response& res){
        if (pom_running) {
            res.set_content("{\"ok\":false, \"msg\":\"already running\"}", "application/json");
            return;
        }
        // parse duration minutes
        int dur = 25;
        auto pos = req.body.find("\"duration\"");
        if (pos != string::npos) {
            // crude parse
            auto p2 = req.body.find_first_of("0123456789", pos);
            if (p2 != string::npos) {
                dur = stoi(req.body.substr(p2));
            }
        }
        pom_running = true;
        pom_start = time(nullptr);
        pom_duration = dur;
        string t = now_iso();
        string sql = "INSERT INTO pomodoros(started_at,duration_min) VALUES('" + t + "'," + to_string(dur) + ");";
        gdb.exec_stmt(sql);
        string rec = "INSERT INTO records(type,note,at) VALUES('pomodoro','start " + to_string(dur) + "m','" + t + "');";
        gdb.exec_stmt(rec);
        // spawn thread to auto-stop after duration. This is optional simple implementation.
        thread([dur]() {
            this_thread::sleep_for(chrono::minutes(dur));
            if (pom_running) {
                pom_running = false;
                string t2 = now_iso();
                string sql = "UPDATE pomodoros SET stopped_at = '" + t2 + "' WHERE id = (SELECT id FROM pomodoros ORDER BY id DESC LIMIT 1);";
                gdb.exec_stmt(sql);
                string rec = "INSERT INTO records(type,note,at) VALUES('pomodoro','auto stop','" + t2 + "');";
                gdb.exec_stmt(rec);
            }
        }).detach();
        res.set_content("{\"ok\":true}", "application/json");
    });

    // Pomodoro stop
    svr.Post("/api/pomodoro/stop", [](const Request&, Response& res){
        if (!pom_running) {
            res.set_content("{\"ok\":false, \"msg\":\"not running\"}", "application/json");
            return;
        }
        pom_running = false;
        string t = now_iso();
        string sql = "UPDATE pomodoros SET stopped_at = '" + t + "' WHERE id = (SELECT id FROM pomodoros ORDER BY id DESC LIMIT 1);";
        gdb.exec_stmt(sql);
        string rec = "INSERT INTO records(type,note,at) VALUES('pomodoro','stop','" + t + "');";
        gdb.exec_stmt(rec);
        res.set_content("{\"ok\":true}", "application/json");
    });

    // records
    svr.Get("/api/records", [](const Request&, Response& res){
        auto rows = gdb.query_rows("SELECT id,type,note,at FROM records ORDER BY id DESC LIMIT 50;");
        string out = "[";
        bool first = true;
        for (auto &r: rows) {
            if (!first) out += ",";
            first = false;
            out += "{";
            out += "\"id\":" + r["id"] + ",";
            out += "\"type\":\"" + r["type"] + "\",";
            out += "\"note\":\"" + r["note"] + "\",";
            out += "\"at\":\"" + r["at"] + "\"";
            out += "}";
        }
        out += "]";
        res.set_content(out, "application/json");
    });

    // dashboard summary
    svr.Get("/api/dashboard", [](const Request&, Response& res){
        // tasks done today
        string today = now_iso().substr(0,10);
        string q1 = "SELECT COUNT(*) as c FROM tasks WHERE done=1 AND substr(created_at,1,10) = '" + today + "';";
        auto r1 = gdb.query_rows(q1);
        string c1 = r1.size() ? r1[0]["c"] : "0";
        // pomos today
        string q2 = "SELECT COUNT(*) as c FROM pomodoros WHERE substr(started_at,1,10) = '" + today + "';";
        auto r2 = gdb.query_rows(q2);
        string c2 = r2.size() ? r2[0]["c"] : "0";
        string out = "{ \"tasks_done_today\": " + c1 + ", \"pomos_today\": " + c2 + " }";
        res.set_content(out, "application/json");
    });

    // suggestions simple logic
    svr.Get("/api/suggestions", [](const Request&, Response& res){
        // example logic: if few pomos today, suggest focus. if many undone tasks, suggest small task
        string today = now_iso().substr(0,10);
        auto r2 = gdb.query_rows("SELECT COUNT(*) as c FROM pomodoros WHERE substr(started_at,1,10) = '" + today + "';");
        int pomos = r2.size() ? stoi(r2[0]["c"]) : 0;
        auto r3 = gdb.query_rows("SELECT COUNT(*) as c FROM tasks WHERE done=0;");
        int undone = r3.size() ? stoi(r3[0]["c"]) : 0;
        vector<string> s;
        if (pomos < 2) s.push_back("Mulai sesi fokus 25 menit untuk menyelesaikan tugas penting");
        if (undone > 5) s.push_back("Pilih 1 tugas kecil dan selesaikan sekarang");
        if (pomos >= 4) s.push_back("Istirahat panjang 15-30 menit");
        if (s.empty()) s.push_back("Review daftar tugas dan tetapkan prioritas 3 tugas teratas");
        // build json array
        string out = "[";
        for (size_t i=0;i<s.size();++i) {
            if (i) out += ",";
            out += "\"" + s[i] + "\"";
        }
        out += "]";
        res.set_content(out, "application/json");
    });

    cout << "Server running at http://localhost:8080\n";
    svr.listen("0.0.0.0", 8080);
    return 0;
}
