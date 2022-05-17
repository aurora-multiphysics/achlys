# import mms
from ftplib import error_reply
import unittest
from mooseutils import fuzzyAbsoluteEqual, find_moose_executable_recursive
import os
import yaml
import json
from subprocess import Popen, PIPE, DEVNULL

achlys_exe = find_moose_executable_recursive(os.getcwd())


def run_executable(app_path, *args, mpi=None, suppress_output=False):
    """
    A function for running an application.
    """
    import subprocess
    if mpi and isinstance(mpi, int):
        cmd = ['mpiexec', '-n', str(mpi), app_path]
    else:
        cmd = [app_path]
    cmd += args
    kwargs = dict(encoding='utf-8')
    if suppress_output:
        kwargs['stdout'] = DEVNULL
        kwargs['stderr'] = DEVNULL
    else:
        kwargs['stdout'] = PIPE
        kwargs['stderr'] = PIPE
    p = subprocess.Popen(cmd, **kwargs)
    output, err = p.communicate()
    if err:
        print(err)
    if output:
        return output.split('\n')


def run_achlys(input_file, *args, **kwargs):
    cli_args = ['-i'] + [input_file]
    cli_args += args
    output  = run_executable(achlys_exe, *cli_args, **kwargs)
    return output


# with open("test_input_parsing.yml") as file:
#         config = yaml.safe_load(file)

class TestArgumentParsing(unittest.TestCase):

    @classmethod
    def setUpClass(cls) -> None:
        with open("test_input_parsing.yml") as file:
            cls.config = yaml.safe_load(file)
        
        cls.object_lists = {}
        for input in cls.config:
            # run_achlys(input, suppress_output=True)
            objects = run_achlys(input, "--list-constructed-objects")
            cls.object_lists[input] = objects

    def compare_results(self, key):
        for input in self.config:
            json_file = input.replace('.i', '_out.json')
            with open(json_file) as file:
                output = json.load(file)
            for block in output:
                assert output[block][key] == self.config[input][block][key]
        
    def test_variable_order(self):
        self.compare_results("variable_order")

    def test_energy_units(self):
        self.compare_results("energy_units")
        self.compare_results("k")

    def test_aux_variables(self):
        self.compare_results("aux_variable_names")
        # self.compare_results("requested_aux_variables")

    def test_solubility(self):
        self.compare_results("solubility_specified")
        self.compare_results("S0")
        self.compare_results("Es")

    def test_density(self):
        self.compare_results("rho")

    def test_custom_base_names(self):
        self.compare_results("mobile_variable_base")
        # self.compare_results("mobile_variable_base")

    def test_all_variable_names(self):
        self.compare_results("all_variable_names")

    def test_solid_boundaries(self):
        self.compare_results("solid_boundaries")

    def test_all_objects_created(self):
        for input in self.config:
            for obj in self.config[input]["objects"]:
                assert obj in self.object_lists[input]

# class TestOutflow(unittest.TestCase):
#     def test(self):
#         df1 = mms.run_spatial('advection-outflow.i', 7, y_pp=['L2u', 'L2v'])
#         fig = mms.ConvergencePlot(xlabel='Element Size ($h$)', ylabel='$L_2$ Error')
#         fig.plot(df1,
#                  label=['L2u', 'L2v'],
#                  marker='o',
#                  markersize=8,
#                  num_fitted_points=3,
#                  slope_precision=1)
#         fig.save('outflow.png')
#         for label,value in fig.label_to_slope.items():
#             if label == 'L2u':
#                 self.assertTrue(fuzzyAbsoluteEqual(value, 1., .05))
#             else:
#                 self.assertTrue(fuzzyAbsoluteEqual(value, 2., .05))

# class TestExtrapolation(unittest.TestCase):
#     def test(self):
#         df1 = mms.run_spatial('advection.i', 7, y_pp=['L2u', 'L2v'])
#         fig = mms.ConvergencePlot(xlabel='Element Size ($h$)', ylabel='$L_2$ Error')
#         fig.plot(df1,
#                  label=['L2u', 'L2v'],
#                  marker='o',
#                  markersize=8,
#                  num_fitted_points=3,
#                  slope_precision=1)
#         fig.save('extrapolation.png')
#         for label,value in fig.label_to_slope.items():
#             if label == 'L2u':
#                 self.assertTrue(fuzzyAbsoluteEqual(value, 1., .05))
#             else:
#                 self.assertTrue(fuzzyAbsoluteEqual(value, 2., .05))

# class UpwindLimiter(unittest.TestCase):
#     def test(self):
#         df1 = mms.run_spatial('limited-advection.i', 7, "FVKernels/advection_u/limiter='upwind'", y_pp=['L2u'])
#         fig = mms.ConvergencePlot(xlabel='Element Size ($h$)', ylabel='$L_2$ Error')
#         fig.plot(df1,
#                  label=['L2u'],
#                  marker='o',
#                  markersize=8,
#                  num_fitted_points=3,
#                  slope_precision=1)
#         fig.save('upwind-limiter.png')
#         for label,value in fig.label_to_slope.items():
#             self.assertTrue(fuzzyAbsoluteEqual(value, 1., .05))

# class CentralDifferenceLimiter(unittest.TestCase):
#     def test(self):
#         df1 = mms.run_spatial('limited-advection.i', 7, "FVKernels/advection_u/limiter='central_difference'", y_pp=['L2u'])
#         fig = mms.ConvergencePlot(xlabel='Element Size ($h$)', ylabel='$L_2$ Error')
#         fig.plot(df1,
#                  label=['L2u'],
#                  marker='o',
#                  markersize=8,
#                  num_fitted_points=3,
#                  slope_precision=1)
#         fig.save('cd-limiter.png')
#         for label,value in fig.label_to_slope.items():
#             self.assertTrue(fuzzyAbsoluteEqual(value, 2., .05))

# class VanLeerLimiter(unittest.TestCase):
#     def test(self):
#         df1 = mms.run_spatial('limited-advection.i', 9, "FVKernels/advection_u/limiter='vanLeer'", y_pp=['L2u'])
#         fig = mms.ConvergencePlot(xlabel='Element Size ($h$)', ylabel='$L_2$ Error')
#         fig.plot(df1,
#                  label=['L2u'],
#                  marker='o',
#                  markersize=8,
#                  num_fitted_points=3,
#                  slope_precision=1)
#         fig.save('vanLeer-limiter.png')
#         for label,value in fig.label_to_slope.items():
#             self.assertTrue(fuzzyAbsoluteEqual(value, 2., .05))

# class MinModLimiter(unittest.TestCase):
#     def test(self):
#         df1 = mms.run_spatial('limited-advection.i', 9, "FVKernels/advection_u/limiter='min_mod'", y_pp=['L2u'])
#         fig = mms.ConvergencePlot(xlabel='Element Size ($h$)', ylabel='$L_2$ Error')
#         fig.plot(df1,
#                  label=['L2u'],
#                  marker='o',
#                  markersize=8,
#                  num_fitted_points=3,
#                  slope_precision=1)
#         fig.save('min-mod-limiter.png')
#         for label,value in fig.label_to_slope.items():
#             self.assertTrue(fuzzyAbsoluteEqual(value, 2., .05))

# class SOULimiter(unittest.TestCase):
#     def test(self):
#         df1 = mms.run_spatial('limited-advection.i', 9, "FVKernels/advection_u/limiter='sou'", y_pp=['L2u'])
#         fig = mms.ConvergencePlot(xlabel='Element Size ($h$)', ylabel='$L_2$ Error')
#         fig.plot(df1,
#                  label=['L2u'],
#                  marker='o',
#                  markersize=8,
#                  num_fitted_points=3,
#                  slope_precision=1)
#         fig.save('sou-limiter.png')
#         for label,value in fig.label_to_slope.items():
#             self.assertTrue(fuzzyAbsoluteEqual(value, 2., .05))

# class QUICKLimiter(unittest.TestCase):
#     def test(self):
#         df1 = mms.run_spatial('limited-advection.i', 15, "FVKernels/advection_u/limiter='quick'", y_pp=['L2u'])
#         fig = mms.ConvergencePlot(xlabel='Element Size ($h$)', ylabel='$L_2$ Error')
#         fig.plot(df1,
#                  label=['L2u'],
#                  marker='o',
#                  markersize=8,
#                  num_fitted_points=3,
#                  slope_precision=1)
#         fig.save('quick-limiter.png')
#         for label,value in fig.label_to_slope.items():
#             self.assertTrue(fuzzyAbsoluteEqual(value, 2., .05))

# class KTLimitedCD(unittest.TestCase):
#     def test(self):
#         df1 = mms.run_spatial('kt-limited-advection.i', 11, "FVKernels/advection_u/limiter='central_difference'", y_pp=['L2u'])
#         fig = mms.ConvergencePlot(xlabel='Element Size ($h$)', ylabel='$L_2$ Error')
#         fig.plot(df1,
#                  label=['L2u'],
#                  marker='o',
#                  markersize=8,
#                  num_fitted_points=3,
#                  slope_precision=1)
#         fig.save('kt-cd-limiter.png')
#         for key,value in fig.label_to_slope.items():
#             self.assertTrue(fuzzyAbsoluteEqual(value, 2., .05))
#             print("%s slope, %f" % (key, value))

# class KTLimitedUpwind(unittest.TestCase):
#     def test(self):
#         df1 = mms.run_spatial('kt-limited-advection.i', 13, "FVKernels/advection_u/limiter='upwind'", y_pp=['L2u'])
#         fig = mms.ConvergencePlot(xlabel='Element Size ($h$)', ylabel='$L_2$ Error')
#         fig.plot(df1,
#                  label=['L2u'],
#                  marker='o',
#                  markersize=8,
#                  num_fitted_points=3,
#                  slope_precision=1)
#         fig.save('kt-upwind-limiter.png')
#         for key,value in fig.label_to_slope.items():
#             self.assertTrue(fuzzyAbsoluteEqual(value, 1., .05))
#             print("%s slope, %f" % (key, value))

# class KTLimitedVanLeer(unittest.TestCase):
#     def test(self):
#         df1 = mms.run_spatial('kt-limited-advection.i', 11, "FVKernels/advection_u/limiter='vanLeer'", y_pp=['L2u'])
#         fig = mms.ConvergencePlot(xlabel='Element Size ($h$)', ylabel='$L_2$ Error')
#         fig.plot(df1,
#                  label=['L2u'],
#                  marker='o',
#                  markersize=8,
#                  num_fitted_points=3,
#                  slope_precision=1)
#         fig.save('kt-van-leer-limiter.png')
#         for key,value in fig.label_to_slope.items():
#             self.assertTrue(fuzzyAbsoluteEqual(value, 2.5, .05))
#             print("%s slope, %f" % (key, value))
