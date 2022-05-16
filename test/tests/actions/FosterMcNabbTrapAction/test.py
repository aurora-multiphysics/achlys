# import mms
# import unittest
# from mooseutils import fuzzyAbsoluteEqual, find_moose_executable_recursive, run_executable
# import os
# import yaml
# import json

# achlys_exe = find_moose_executable_recursive(os.getcwd())

# def run_achlys(input_file, *args):
#     cli_args = ['-i'] + input_file
#     cli_args += args
#     _  = run_executable(achlys_exe, *cli_args)

# class TestArgumentParsing(unittest.TestCase):

#     @classmethod
#     def setUpClass(cls) -> None:
#         with open(test_input_parsing.yml) as file:
#             cls.config = yaml.safe_load(file)
        
#         for input in config:
#             run_achlys(input, "--list-constructed-objects")

#     def compare_results(self, key):
#         for input in self.config:
#             json_file = input.replace('.i', '_out.json')
#             with open(json_file) as file:
#                 output = json.loads(file)
#             assert output[key] == self.config[input][key]
        
#     def test_variable_order(self):
#         self.compare_results("variable_order")

#     def test_energy_units(self):
#         self.compare_results("energy_units")

#     def test_aux_variables(self):
#         self.compare_results("energy_units")

#     def test_solubility(self):
#         self.compare_results("energy_units")

#     def test_density(self):
#         self.compare_results("energy_units")

#     def test_custom_base_names(self):
#         self.compare_results("energy_units")



# # class TestOutflow(unittest.TestCase):
# #     def test(self):
# #         df1 = mms.run_spatial('advection-outflow.i', 7, y_pp=['L2u', 'L2v'])
# #         fig = mms.ConvergencePlot(xlabel='Element Size ($h$)', ylabel='$L_2$ Error')
# #         fig.plot(df1,
# #                  label=['L2u', 'L2v'],
# #                  marker='o',
# #                  markersize=8,
# #                  num_fitted_points=3,
# #                  slope_precision=1)
# #         fig.save('outflow.png')
# #         for label,value in fig.label_to_slope.items():
# #             if label == 'L2u':
# #                 self.assertTrue(fuzzyAbsoluteEqual(value, 1., .05))
# #             else:
# #                 self.assertTrue(fuzzyAbsoluteEqual(value, 2., .05))


