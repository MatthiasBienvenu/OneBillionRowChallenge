import os
import sys

import numpy as np
import pandas as pd

MEASUREMENTS_PER_PASS = 100_000_000

input_file = "data/cities.csv"
nb_measurements = int(sys.argv[1]) if len(sys.argv) > 1 else 10000

if len(sys.argv) > 2:
    output_file = sys.argv[2]
    mode = "a"

    if os.path.exists(output_file):
        if os.path.isfile(output_file):
            os.remove(output_file)
        else:
            raise IsADirectoryError(output_file)
else:
    output_file = sys.stdout
    mode = "w"

cities = pd.read_csv(input_file, sep=";")

print("Generating " + str(nb_measurements) + " data points in " + str(output_file))

while nb_measurements > 0:
    print(f"still {nb_measurements} measurements to do")
    measurements = cities.sample(
        min(nb_measurements, MEASUREMENTS_PER_PASS), replace=True
    )

    nb_measurements -= MEASUREMENTS_PER_PASS

    print("  Picking random cities: OK")
    measurements["Mean_temperature"].astype(float)
    measurements["Temperature"] = np.random.normal(measurements["Mean_temperature"], 5)
    print("  Generating random temperature: OK")

    # only keep 1 digit
    measurements["Temperature"] = (measurements["Temperature"] * 10).astype(int) / 10
    measurements = measurements.drop(columns=["Mean_temperature"])

    print("  Writing the output file...")
    measurements.to_csv(output_file, header=False, index=False, sep=";", mode=mode)

print("  Writing the output file: OK")
