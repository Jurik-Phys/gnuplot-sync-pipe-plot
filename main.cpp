#include <iostream>
#include <cmath>
#include <cstdio>
#include <unistd.h>
#include <vector>

int main(int argc, char** argv){

    // Open pipe for communication with gnuplot
    FILE *plotPipe = popen("gnuplot --persist", "w");

    if (!plotPipe){
        std::cerr << "[E] GnuPlot pipe error. Exit!" << "\n";
        return 1;
    }

    fprintf(plotPipe, "set term wxt noraise\n");
    fprintf(plotPipe, "set title 'Animated graph'\n");
    fprintf(plotPipe, "set xlabel 'Time'\n");
    fprintf(plotPipe, "set yrange[-1.3:1.3]\n");
    fprintf(plotPipe, "set ylabel 'Function'\n");

    srand(time(NULL));

    const int frameResolution = 400;
    std::vector<double> timeArray;
    std::vector<double> funcArray;

    const double dt = 0.1;
    double t = 0.0;
    double funcValue = 0.0;
    const double endTime = 5000.0;

    for (int idx = 0; idx < int(endTime/dt); idx++){

        fprintf(plotPipe, "set xrange[%f:%f]\n", (idx-0.70*frameResolution)*dt, (idx+0.30*frameResolution)*dt);
        fprintf(plotPipe, "plot '-' with lines\n");

        t = idx*dt;
        funcValue = sin(t) + 0.3*rand()/(double)RAND_MAX;

        if (idx < frameResolution){
            timeArray.push_back(t);
            funcArray.push_back(funcValue);
        }
        else {
            timeArray.erase(timeArray.begin());
            funcArray.erase(funcArray.begin());
            timeArray.push_back(t);
            funcArray.push_back(funcValue);
        }

        // Send data to gnuplot
        for (int i = 0; i < timeArray.size(); i++){
            fprintf(plotPipe, "%f %f\n", timeArray[i], funcArray[i]);
            fflush(plotPipe);
        }
        fprintf(plotPipe, "e\n");
        fflush(plotPipe);

        // Пауза в микросекундах (100000 микросекунд = 0.1 секунды)
        usleep(10000);
    }
    pclose(plotPipe);
    return 0;
}
