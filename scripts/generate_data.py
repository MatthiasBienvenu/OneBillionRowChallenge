import sys

import numpy as np
import pandas as pd

input_file = "data/cities.csv"
output_file = "data/measurements.csv"
nb_measurements = 10000

if len(sys.argv) > 1:
    nb_measurements = int(sys.argv[1])

cities = pd.read_csv(input_file, sep=";")
nb_selected_cities = len(cities)
if len(sys.argv) > 2:
    nb_selected_cities = int(sys.argv[2])
    cities = cities.sample(nb_selected_cities)

print("Generating " + str(nb_measurements) + " data points in " + output_file)

measurements = cities.sample(nb_measurements, replace=True)
print("  Picking random cities: OK")
measurements["Mean_temperature"].astype(float)
measurements["Temperature"] = np.random.normal(measurements["Mean_temperature"], 5)
print("  Generating random temperature: OK")

# only keep 1 digit
measurements["Temperature"] = (measurements["Temperature"] * 10).astype(int) / 10
measurements = measurements.drop(columns=["Mean_temperature"])

print("  Writing the output file...")
measurements.to_csv(output_file, header=False, index=False, sep=";")
print("  Writing the output file: OK")
print("  min temperature: " + str(min(measurements["Temperature"])))
print("  max temperature: " + str(max(measurements["Temperature"])))
