sh run.sh
if [ -f "timing_results.csv" ]; then
    rm "timing_results.csv"
fi
touch timing_results.csv
sh job1.sh
sleep 3
sh plot.sh
