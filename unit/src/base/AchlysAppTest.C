#include "gtest/gtest.h"
#include <memory>
#include "AchlysAppTest.h"

// google test fixture macro
TEST_F(AchlysAppBasicTest, registryTest)
{
    using ObjectVector = std::vector<std::string>;

    ObjectVector kernels{"ADTrappingEquilibriumEquation"};
    check_object_registration(kernels, "achlysApp");

    ObjectVector materials{"ArrheniusMaterial"};
    check_object_registration(materials, "achlysApp");

    ObjectVector interface_kernels{"ADChemicalPotentialInterface",
                                   "ADVariableMatch",
                                   "ADMatInterfaceDiffusion"
                                   };
    check_object_registration(interface_kernels, "achlysApp");


    // using ActionList = std::map<std::string, std::vector<std::string>>
    // actions registered to different warehouse
    std::vector<std::map<std::string, std::vector<std::string>>> actions{
                                        {{"DiffusiveMaterialAction", 
                                                {
                                                    "add_kernels",
                                                    "add_variables",
                                                    "add_materials",
                                                    "add_interface_kernels",
                                                    "add_aux_variables",
                                                    "add_aux_kernels"
                                                }
                                        }},
                                        {{"FosterMcNabbTrapAction", 
                                            {
                                                "add_kernels",
                                                "add_variables",
                                                "add_materials",
                                                "add_interface_kernels",
                                                "add_aux_variables",
                                                "add_aux_kernels"
                                            }
                                        }}
                                    };

    check_app_action_registration(actions, "achlysApp");
}