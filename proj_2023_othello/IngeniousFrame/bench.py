import sys
import os
import time
import re
import subprocess
import threading
import json
import uuid


JAVA = "/opt/homebrew/Cellar/openjdk@17/17.0.7/bin/java"
FRAMEWORK_JAR = "IngeniousFramework.jar"
ADD_OPENS = "--add-opens java.base/java.util=ALL-UNNAMED --add-opens java.desktop/java.awt=ALL-UNNAMED"
GAMERESULTS = []
regexnd = r"INFO: You \(.*?\) (?P<res>.*)"
regexd = r"INFO: It's a draw!"




def kill_procs(file = sys.stdout):
    try:
        pids = subprocess.check_output("lsof -t -i:61235", shell=True)
        if pids:
            print("Killing Old Server", file = file)
            subprocess.call("kill " + str(pids.decode().strip()), shell=True)
            time.sleep(2)
    except:
        print("No Live Servers Found", file = file)
        pass


def run_command(command, print_output=False, file=sys.stdout):
    subproc = subprocess.Popen(
        command,
        shell=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE
    )
    if print_output:
        output = ""
        while subproc.poll() is None:
            line = subproc.stdout.readline().decode()
            output += line
            if line:
                print(line.strip(), file=file, flush = True)
    else:
        output = subproc.stdout.read().decode()
    error = subproc.stderr.read().decode()
    exitCode = subproc.wait()
    subproc.kill()
    return output, error, exitCode


def makePlayer(file=sys.stdout):
    os.chdir("src_my_player")
    run_command("make clean")
    _, _, exitCode = run_command("make", file=file)
    if (exitCode != 0):
        print("make failed", file=file)
        sys.exit(0)
    run_command("make clean")
    os.chdir("..")


def makeRandomPlayer(file=sys.stdout):
    os.chdir("src_random_player")
    run_command("make clean", file=file)
    _, _, exitCode = run_command("make", file=file)
    if (exitCode != 0):
        print("make random failed", file=file)
        sys.exit(0)
    run_command("make clean")
    os.chdir("..")


def moveLogs(file=sys.stdout):
    print("Moving log output to the Logs directory", file = file)
    run_command("mv *.txt Logs/", file = file)


def startServer(file = sys.stdout):
    # java -jar IngeniousFramework.jar server -port 61235
    output, error, exitCode = run_command(
        f"{JAVA} -jar {ADD_OPENS} {FRAMEWORK_JAR} server -port 61235", file = file)


def startLobby(lid, file = sys.stdout):
    output, error, exitCode = run_command(
        f"{JAVA} -jar {ADD_OPENS} {FRAMEWORK_JAR} create -config \"Othello.json\" -game \"OthelloReferee\" -lobby \"mylobby-" + str(lid) + "\" -hostname localhost -port 61235",file = file)


def startPlayer(player, lid, file=sys.stdout):
    printout = False
    if player == "my_player":
        printout = True
    output, error, exitCode = run_command(f"{JAVA} -jar {ADD_OPENS} {FRAMEWORK_JAR} client -config \"Othello.json\" -username " + str(
        player) + " -engine za.ac.sun.cs.ingenious.games.othello.engines.OthelloMPIEngine -game OthelloReferee -hostname localhost -lobby \"mylobby-" + str(lid) + "\" -port 61235", printout,file = file)
    if printout:
        matches = re.findall(regexnd, output, re.MULTILINE)
        if len(matches) == 0:
            matches = re.findall(regexd, output, re.MULTILINE)
            if len(matches) == 0:
                print("Error finding Result!", file=file)
            else:
                GAMERESULTS.append(0.5)
                print("GAME WAS A DRAW!", file=file)
        else:
            print(matches, file = file)
            for match in matches:
                if "won" in match:
                    GAMERESULTS.append(1)
                    print("GAME WAS WON!", file=file)
                elif "lost" in match:
                    GAMERESULTS.append(0)
                    print("GAME WAS LOST!", file=file)
                else:
                    GAMERESULTS.append(0.5)
                    print("GAME WAS A DRAW!", file=file)


def writeGameConf(p1, p2):
    game = {
        "numPlayers": 2,
        "threads": 4,
        "boardSize": 8,
        "time": 4,
        "turnLength": 4000,
        "path1": p1,
        "path2": p2
    }
    with open("Othello.json", "w") as outfile:
        json.dump(game, outfile, indent=4)


def runTests(directory="players", file=sys.stdout):
    global GAMERESULTS
    print("Starting Server and Lobby...",file=file)
    t_server = threading.Thread(target=startServer,kwargs={'file':file})
    t_server.daemon = True
    t_server.start()
    time.sleep(2)
    GAMERESULTS = []
    for filename in os.listdir(directory):
        f = os.path.join(directory, filename)
        if os.path.isfile(f):
            if (f == "players/my_player"):
                continue

        print("Match of " + f + " vs my_player", file=file)
        writeGameConf(f, "players/my_player")
        lid = str(uuid.uuid4())
        t_lobby = threading.Thread(target=startLobby, args=(lid,), kwargs={'file': file})
        t_lobby.start()
        time.sleep(2)
        t_player1 = threading.Thread(target=startPlayer, args=(filename, lid,),kwargs={'file': file})
        t_player2 = threading.Thread(
            target=startPlayer, args=("my_player", lid,),kwargs={'file': file})
        t_player1.start()
        time.sleep(1)
        t_player2.start()
        t_player1.join()
        t_player2.join()
        t_lobby.join()

        print("Match of my_player vs " + f,file=file)
        writeGameConf("players/my_player", f)
        lid = str(uuid.uuid4())
        t_lobby = threading.Thread(target=startLobby, args=(lid,),kwargs={'file': file})
        t_lobby.start()
        time.sleep(2)
        t_player1 = threading.Thread(
            target=startPlayer, args=("my_player", lid,),kwargs={'file': file})
        t_player2 = threading.Thread(target=startPlayer, args=(filename, lid,),kwargs={'file': file})
        t_player1.start()
        time.sleep(1)
        t_player2.start()
        t_player1.join()
        t_player2.join()
        t_lobby.join()

    kill_procs(file = file)
    moveLogs(file = file)
    print("Done", file = file)
    print(GAMERESULTS, file = file)


def make(file = sys.stdout):
    directory = "players"
    if not os.path.exists(directory):
        os.mkdir(directory)
    print("Making player...", file = file)
    makePlayer(file = file)
    print("Making Random Player...", file = file)
    makeRandomPlayer(file = file)


def interpret_data(num_files, times):
    w_wins = 0
    w_losses = 0
    w_draws = 0
    b_wins = 0
    b_losses = 0
    b_draws = 0
    matchCount = 0

    for i in range(num_files):
        with open(f"benchmarks/run{i + 1}.txt") as f:
            lines = f.readlines()
            pls = re.findall("Match of ([\\w/_\\-]+) vs ([\\w/_\\-]+)","".join(lines))
            
            score = [float(r) for r in re.findall("\\[\\s*([\\d.]+)(?:,\\s*([\\d.]+))\\s*\\]",lines[-1])[0]]
            for (black, white), scr in zip(pls,score):
                if white == "my_player":
                    if (scr == 0):
                        w_losses += 1
                    if (scr == 0.5):
                        w_draws += 1
                    if (scr == 1):
                        w_wins += 1
                elif black == "my_player":
                    if (scr == 0):
                        b_losses += 1
                    if (scr == 0.5):
                        b_draws += 1
                    if (scr == 1):
                        b_wins += 1
                matchCount += 1

    runtimes_output = open("benchmarks/runtimes.txt", "w")
    output = open("benchmarks/results.txt", "w")

    w_stats = f"white: \t{w_wins} wins | {w_losses} losses | {w_draws} draws"
    b_stats = f"black: \t{b_wins} wins | {b_losses} losses | {b_draws} draws"
    total_stats = f"total: \t{w_wins + b_wins} wins"
    total_stats = f"{total_stats} | {w_losses + b_losses} losses"
    total_stats = f"{total_stats} | {w_draws + b_draws} draws"
    total_stats = f"{total_stats} | {matchCount} matches"

    print(w_stats)
    print(b_stats)
    print(total_stats)
    output.write(w_stats)
    output.write("\n")
    output.write(b_stats)
    output.write("\n")
    output.write(total_stats)
    output.write("\n")

    w_ratio = round(((w_wins)/(matchCount/2))*100, 2)
    w_ratio_string = f"white win ratio: \t{w_ratio}%"
    b_ratio = round(((b_wins)/(matchCount/2))*100, 2)
    b_ratio_string = f"black win ratio: \t{b_ratio}%"
    total_ratio = round(((w_wins + b_wins)/matchCount)*100, 2)
    total_ratio_string = f"total win ratio: \t{total_ratio}%"
    print(w_ratio_string)
    output.write(w_ratio_string)
    output.write("\n")
    print(b_ratio_string)
    output.write(b_ratio_string)
    output.write("\n")
    print(total_ratio_string)
    output.write(total_ratio_string)
    output.write("\n")

    total_time = 0
    i = 1
    for t in times:
        runtimes_output.write(f"run {i}: {round(t, 2)} s")
        runtimes_output.write("\n")
        total_time += t
        i += 1
    avg_time = round(total_time/len(times), 2)
    avg_time_string = f"  per run: \t\t{avg_time} s"
    avg_ind_time = round(total_time/matchCount, 2)
    avg_ind_time_string = f"  per match: \t\t{avg_ind_time} s"

    print("average runtime:")
    output.write("average runtime:\n")

    print(avg_time_string)
    output.write(avg_time_string)
    output.write("\n")

    print(avg_ind_time_string)
    output.write(avg_ind_time_string)
    output.write("\n")

    total_time = round(total_time, 2)
    total_time_string = f"total runtime: \t\t{total_time} s"
    print(total_time_string)
    output.write(total_time_string)
    output.write("\n")

    runtimes_output.flush()
    output.flush()
    runtimes_output.close()
    output.close()
def checkJAVA():
    global JAVA
    if not os.path.exists(JAVA):
        JAVA = "java"
    try:
        subprocess.call([JAVA, "--version"],stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    except FileNotFoundError:
        print("Java is not installed?!?")
        exit(0)

def main():
    num_tests = int(sys.argv[1])
    checkJAVA()
    print("Cleaning...", end="\r")
    if not os.path.exists("benchmarks"):
        os.mkdir("benchmarks")
    for f in os.listdir("benchmarks"):
        if f.endswith(".txt"):
            os.remove(os.path.join("benchmarks",f))

    subprocess.call([sys.executable,"run_clean.py"],stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL)

    times = [0 for _ in range(num_tests)]
    make()
    for index in range(num_tests):
        test_num = index + 1
        print(f"Running test {test_num}/{num_tests}", end="\r",file=sys.stdout)

        # Run the tournament      
        with open(f"benchmarks/run{test_num}.txt", "tw") as file:
            start = time.time()
            runTests(file = file)
            end = time.time()
        times[index] = (end - start)

    # Interpret results
    interpret_data(num_tests, times)

if __name__ == "__main__":
    main()
    
