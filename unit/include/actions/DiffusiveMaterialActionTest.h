#pragma once

#include "AchlysAppTest.h"
#include "Executioner.h"
#include "FEProblemBase.h"


class DiffusiveMaterialActionTest : public AchlysAppInputTest{
protected:

  DiffusiveMaterialActionTest() : AchlysAppInputTest("transfer.i") {};
  DiffusiveMaterialActionTest(std::string inputfile) : AchlysAppInputTest(inputfile) {};

  virtual void SetUp() override {

    // Call the base class method
    ASSERT_NO_THROW(AchlysAppInputTest::SetUp());

    ASSERT_FALSE(appIsNull);
    ASSERT_NO_THROW(app->setupOptions());
    ASSERT_NO_THROW(app->runInputFile());

    // Get the executioner
    executionerPtr = app->getExecutioner();
    ASSERT_NE(executionerPtr,nullptr);

    // Initialise (sets up multiapp and transfers)
    ASSERT_NO_THROW(executionerPtr->init());

    // Get the FE problem
    problemPtr = &(executionerPtr->feProblem());
    ASSERT_NE(problemPtr,nullptr);

    // Check transfers
    std::vector< std::shared_ptr< Transfer > > transfers_from =
      problemPtr->getTransfers(Transfer::DIRECTION::FROM_MULTIAPP);
    EXPECT_EQ(transfers_from.size(),size_t(0));

    // std::vector< std::shared_ptr< Transfer > > transfers_to =
    //   problemPtr->getTransfers(Transfer::DIRECTION::TO_MULTIAPP);
    // ASSERT_EQ(transfers_to.size(),size_t(1));

    // // Upcast transfer ptr
    // mbMeshTransferPtr =
    //   std::dynamic_pointer_cast<MoabMeshTransfer>(transfers_to.back());

    // // Get multiapp ptr
    // std::shared_ptr< MultiApp > multiAppPtr = problemPtr->getMultiApp("openmc");
    // ASSERT_NE(multiAppPtr, nullptr);

    // // Get sub app ptr
    // ASSERT_TRUE(multiAppPtr->hasApp());
    // ASSERT_EQ(multiAppPtr->numGlobalApps(),size_t(1));
    // ASSERT_TRUE(multiAppPtr->hasLocalApp(0));
    // MooseApp * subAppPtr = multiAppPtr->localApp(0);
    // ASSERT_NE(subAppPtr, nullptr);
    // EXPECT_EQ(subAppPtr->type(),"OpenMCApp");

    // // Get sub problem
    // FEProblemBase* subProblemPtr = &(subAppPtr->getExecutioner()->feProblem());
    // ASSERT_NE(subProblemPtr, nullptr);

    // // Get moab user object
    // subProblemPtr->hasUserObject("moab");
    // moabUOPtr = &(subProblemPtr->getUserObject<MoabUserObject>("moab"));

  }

  Executioner* executionerPtr=nullptr;
  FEProblemBase* problemPtr=nullptr;
//   std::shared_ptr< MoabMeshTransfer > mbMeshTransferPtr;
//   MoabUserObject* moabUOPtr=nullptr;

};