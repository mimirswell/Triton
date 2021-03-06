#include <iostream>
#include <sstream>
#include <stdexcept>

#include <SbbIRBuilder.h>
#include <Registers.h>
#include <SMT2Lib.h>
#include <SymbolicElement.h>


SbbIRBuilder::SbbIRBuilder(uint64 address, const std::string &disassembly):
  BaseIRBuilder(address, disassembly) {
}


void SbbIRBuilder::regImm(AnalysisProcessor &ap, Inst &inst) const {
  SymbolicElement   *se;
  std::stringstream expr, op1, op2, op3;
  uint64            reg     = this->operands[0].getValue();
  uint64            imm     = this->operands[1].getValue();
  uint32            regSize = this->operands[0].getSize();

  /* Create the SMT semantic */
  op1 << ap.buildSymbolicRegOperand(reg, regSize);
  op2 << smt2lib::bv(imm, regSize * REG_SIZE);
  op3 << ap.buildSymbolicFlagOperand(ID_CF, regSize);

  /* Finale expr */
  expr << smt2lib::bvsub(op1.str(), smt2lib::bvadd(op2.str(), op3.str()));

  /* Create the symbolic element */
  se = ap.createRegSE(inst, expr, reg, regSize);

  /* Apply the taint */
  ap.aluSpreadTaintRegImm(se, reg);

  /* Add the symbolic flags element to the current inst */
  EflagsBuilder::af(inst, se, ap, regSize, op1, op2);
  EflagsBuilder::cfSub(inst, se, ap, op1, op2);
  EflagsBuilder::ofSub(inst, se, ap, regSize, op1, op2);
  EflagsBuilder::pf(inst, se, ap);
  EflagsBuilder::sf(inst, se, ap, regSize);
  EflagsBuilder::zf(inst, se, ap, regSize);
}


void SbbIRBuilder::regReg(AnalysisProcessor &ap, Inst &inst) const {
  SymbolicElement   *se;
  std::stringstream expr, op1, op2, op3;
  uint64            reg1     = this->operands[0].getValue();
  uint64            reg2     = this->operands[1].getValue();
  uint32            regSize1 = this->operands[0].getSize();
  uint32            regSize2 = this->operands[1].getSize();

  /* Create the SMT semantic */
  op1 << ap.buildSymbolicRegOperand(reg1, regSize1);
  op2 << ap.buildSymbolicRegOperand(reg2, regSize2);
  op3 << ap.buildSymbolicFlagOperand(ID_CF, regSize1);

  /* Final expr */
  expr << smt2lib::bvsub(op1.str(), smt2lib::bvadd(op2.str(), op3.str()));

  /* Create the symbolic element */
  se = ap.createRegSE(inst, expr, reg1, regSize1);

  /* Apply the taint */
  ap.aluSpreadTaintRegReg(se, reg1, reg2);

  /* Add the symbolic flags element to the current inst */
  EflagsBuilder::af(inst, se, ap, regSize1, op1, op2);
  EflagsBuilder::cfSub(inst, se, ap, op1, op2);
  EflagsBuilder::ofSub(inst, se, ap, regSize1, op1, op2);
  EflagsBuilder::pf(inst, se, ap);
  EflagsBuilder::sf(inst, se, ap, regSize1);
  EflagsBuilder::zf(inst, se, ap, regSize1);
}


void SbbIRBuilder::regMem(AnalysisProcessor &ap, Inst &inst) const {
  SymbolicElement   *se;
  std::stringstream expr, op1, op2, op3;
  uint32            readSize = this->operands[1].getSize();
  uint64            mem      = this->operands[1].getValue();
  uint64            reg      = this->operands[0].getValue();
  uint32            regSize  = this->operands[0].getSize();

  /* Create the SMT semantic */
  op1 << ap.buildSymbolicRegOperand(reg, regSize);
  op2 << ap.buildSymbolicMemOperand(mem, readSize);
  op3 << ap.buildSymbolicFlagOperand(ID_CF, regSize);

  /* Final expr */
  expr << smt2lib::bvsub(op1.str(), smt2lib::bvadd(op2.str(), op3.str()));

  /* Create the symbolic element */
  se = ap.createRegSE(inst, expr, reg, regSize);

  /* Apply the taint */
  ap.aluSpreadTaintRegMem(se, reg, mem, readSize);

  /* Add the symbolic flags element to the current inst */
  EflagsBuilder::af(inst, se, ap, regSize, op1, op2);
  EflagsBuilder::cfSub(inst, se, ap, op1, op2);
  EflagsBuilder::ofSub(inst, se, ap, regSize, op1, op2);
  EflagsBuilder::pf(inst, se, ap);
  EflagsBuilder::sf(inst, se, ap, regSize);
  EflagsBuilder::zf(inst, se, ap, regSize);
}


void SbbIRBuilder::memImm(AnalysisProcessor &ap, Inst &inst) const {
  SymbolicElement   *se;
  std::stringstream expr, op1, op2, op3;
  uint32            writeSize = this->operands[0].getSize();
  uint64            mem       = this->operands[0].getValue();
  uint64            imm       = this->operands[1].getValue();

  /* Create the SMT semantic */
  op1 << ap.buildSymbolicMemOperand(mem, writeSize);
  op2 << smt2lib::bv(imm, writeSize * REG_SIZE);
  op3 << ap.buildSymbolicFlagOperand(ID_CF, writeSize);

  /* Final expr */
  expr << smt2lib::bvsub(op1.str(), smt2lib::bvadd(op2.str(), op3.str()));

  /* Create the symbolic element */
  se = ap.createMemSE(inst, expr, mem, writeSize);

  /* Apply the taint */
  ap.aluSpreadTaintMemImm(se, mem, writeSize);

  /* Add the symbolic flags element to the current inst */
  EflagsBuilder::af(inst, se, ap, writeSize, op1, op2);
  EflagsBuilder::cfSub(inst, se, ap, op1, op2);
  EflagsBuilder::ofSub(inst, se, ap, writeSize, op1, op2);
  EflagsBuilder::pf(inst, se, ap);
  EflagsBuilder::sf(inst, se, ap, writeSize);
  EflagsBuilder::zf(inst, se, ap, writeSize);
}


void SbbIRBuilder::memReg(AnalysisProcessor &ap, Inst &inst) const {
  SymbolicElement   *se;
  std::stringstream expr, op1, op2, op3;
  uint32            writeSize = this->operands[0].getSize();
  uint64            mem       = this->operands[0].getValue();
  uint64            reg       = this->operands[1].getValue();
  uint32            regSize   = this->operands[1].getSize();

  /* Create the SMT semantic */
  op1 << ap.buildSymbolicMemOperand(mem, writeSize);
  op2 << ap.buildSymbolicRegOperand(reg, regSize);
  op3 << ap.buildSymbolicFlagOperand(ID_CF, regSize);

  /* Final expr */
  expr << smt2lib::bvsub(op1.str(), smt2lib::bvadd(op2.str(), op3.str()));

  /* Create the symbolic element */
  se = ap.createMemSE(inst, expr, mem, writeSize);

  /* Apply the taint */
  ap.aluSpreadTaintMemReg(se, mem, reg, writeSize);

  /* Add the symbolic flags element to the current inst */
  EflagsBuilder::af(inst, se, ap, writeSize, op1, op2);
  EflagsBuilder::cfSub(inst, se, ap, op1, op2);
  EflagsBuilder::ofSub(inst, se, ap, writeSize, op1, op2);
  EflagsBuilder::pf(inst, se, ap);
  EflagsBuilder::sf(inst, se, ap, writeSize);
  EflagsBuilder::zf(inst, se, ap, writeSize);
}


Inst *SbbIRBuilder::process(AnalysisProcessor &ap) const {
  this->checkSetup();

  Inst *inst = new Inst(ap.getThreadID(), this->address, this->disas);

  try {
    this->templateMethod(ap, *inst, this->operands, "SBB");
    ap.incNumberOfExpressions(inst->numberOfElements()); /* Used for statistics */
    ControlFlow::rip(*inst, ap, this->nextAddress);
  }
  catch (std::exception &e) {
    delete inst;
    throw;
  }

  return inst;
}

