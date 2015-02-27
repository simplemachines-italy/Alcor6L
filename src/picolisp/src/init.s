# 04oct14abu
# (c) Software Lab. Alexander Burger
#
# 19dec2014
# (c) SimpleMachines. Raman Gopalan

### System Globals ###
@ [At] NIL
@@ [At2] NIL
@@@ [At3] NIL
This [This] NIL
meth [Meth] {doMeth}
*Dbg [Dbg] NIL
*Scl [Scl] 0
*Class [Class] NIL
^ [Up] NIL
*Err [Err] NIL
*Msg [Msg] NIL
#*pio-output* [plisp_pio_output] 0
*Bye [Bye] NIL  # Last unremovable symbol

### System Functions ###
abs {doAbs}
+ {doAdd}
all {doAll}
and {doAnd}
any {doAny}
append {doAppend}
apply {doApply}
arg {doArg}
args {doArgs}
argv {doArgv}
-> {doArrow}
as {doAs}
asoq {doAsoq}
assoc {doAssoc}
at {doAt}
atom {doAtom}
bind {doBind}
& {doBitAnd}
| {doBitOr}
bit? {doBitQ}
x| {doBitXor}
bool {doBool}
box {doBox}
box? {doBoxQ}
! {doBreak}
by {doBy}
bye {doBye}
caaar {doCaaar}
caadr {doCaadr}
caar {doCaar}
cadar {doCadar}
cadddr {doCadddr}
caddr {doCaddr}
cadr {doCadr}
car {doCar}
case {doCase}
casq {doCasq}
catch {doCatch}
cdaar {doCdaar}
cdadr {doCdadr}
cdar {doCdar}
cddar {doCddar}
cddddr {doCddddr}
cdddr {doCdddr}
cddr {doCddr}
cdr {doCdr}
char {doChar}
chain {doChain}
chop {doChop}
circ {doCirc}
circ? {doCircQ}
clip {doClip}
cmd {doCmd}
cnt {doCnt}
: {doCol}
con {doCon}
conc {doConc}
cond {doCond}
cons {doCons}
copy {doCopy}
cut {doCut}
date {doDate}
de {doDe}
dec {doDec}
def {doDef}
default {doDefault}
del {doDel}
delete {doDelete}
delq {doDelq}
diff {doDiff}
/ {doDiv}
dm {doDm}
do {doDo}
e {doE}
env {doEnv}
eof {doEof}
eol {doEol}
== {doEq}
=0 {doEq0}
=T {doEqT}
= {doEqual}
eval {doEval}
extra {doExtra}
extract {doExtract}
fifo {doFifo}
fill {doFill}
filter {doFilter}
fin {doFin}
finally {doFinally}
find {doFind}
fish {doFish}
flg? {doFlgQ}
flip {doFlip}
flush {doFlush}
fold {doFold}
for {doFor}
format {doFormat}
from {doFrom}
full {doFull}
fully {doFully}
fun? {doFunQ}
gc {doGc}
>= {doGe}
ge0 {doGe0}
get {doGet}
getl {doGetl}
glue {doGlue}
> {doGt}
gt0 {doGt0}
head {doHead}
heap {doHeap}
==== {doHide}
idx {doIdx}
if {doIf}
if2 {doIf2}
ifn {doIfn}
in {doIn}
inc {doInc}
index {doIndex}
intern {doIntern}
isa {doIsa}
job {doJob}
last {doLast}
<= {doLe}
le0 {doLe0}
length {doLength}
let {doLet}
let? {doLetQ}
line {doLine}
link {doLink}
list {doList}
lit {doLit}
lst? {doLstQ}
load {doLoad}
loop {doLoop}
low? {doLowQ}
lowc {doLowc}
< {doLt}
lt0 {doLt0}
lup {doLup}
made {doMade}
make {doMake}
map {doMap}
mapc {doMapc}
mapcan {doMapcan}
mapcar {doMapcar}
mapcon {doMapcon}
maplist {doMaplist}
maps {doMaps}
match {doMatch}
max {doMax}
maxi {doMaxi}
member {doMember}
memq {doMemq}
meta {doMeta}
method {doMethod}
min {doMin}
mini {doMini}
mix {doMix}
mmeq {doMmeq}
* {doMul}
*/ {doMulDiv}
name {doName}
nand {doNand}
n== {doNEq}
n0 {doNEq0}
nT {doNEqT}
<> {doNEqual}
need {doNeed}
new {doNew}
next {doNext}
nil {doNil}
nond {doNond}
nor {doNor}
not {doNot}
nth {doNth}
num? {doNumQ}
off {doOff}
offset {doOffset}
on {doOn}
one {doOne}
onOff {doOnOff}
opt {doOpt}
or {doOr}
out {doOut}
pack {doPack}
pair {doPair}
pass {doPass}
path {doPath}
pat? {doPatQ}
peek {doPeek}
pick {doPick}
pop {doPop}
pre? {doPreQ}
prin {doPrin}
prinl {doPrinl}
print {doPrint}
println {doPrintln}
printsp {doPrintsp}
prior {doPrior}
prog {doProg}
prog1 {doProg1}
prog2 {doProg2}
prop {doProp}
:: {doPropCol}
prove {doProve}
push {doPush}
push1 {doPush1}
put {doPut}
putl {doPutl}
queue {doQueue}
quit {doQuit}
rand {doRand}
range {doRange}
rank {doRank}
read {doRead}
% {doRem}
replace {doReplace}
rest {doRest}
reverse {doReverse}
rot {doRot}
run {doRun}
sect {doSect}
seed {doSeed}
seek {doSeek}
; {doSemicol}
send {doSend}
set {doSet}
=: {doSetCol}
setq {doSetq}
>> {doShift}
size {doSize}
skip {doSkip}
sort {doSort}
space {doSpace}
split {doSplit}
sp? {doSpQ}
sqrt {doSqrt}
state {doState}
stem {doStem}
str {doStr}
strip {doStrip}
str? {doStrQ}
- {doSub}
sum {doSum}
super {doSuper}
swap {doSwap}
sym {doSym}
sym? {doSymQ}
t {doT}
tail {doTail}
text {doText}
throw {doThrow}
till {doTill}
$ {doTrace}
trim {doTrim}
try {doTry}
type {doType}
unify {doUnify}
unless {doUnless}
until {doUntil}
up {doUp}
upp? {doUppQ}
uppc {doUppc}
use {doUse}
val {doVal}
when {doWhen}
while {doWhile}
with {doWith}
xchg {doXchg}
xor {doXor}
yoke {doYoke}
zap {doZap}
zero {doZero}

### Microcontroller specific modules ###

### Platform ###
pd-platform {pd_platform}
pd-cpu {pd_cpu}
pd-board {pd_board}

### Terminal ###
term-clrscr {plisp_term_clrscr}
term-clreol {plisp_term_clreol}
term-moveto {plisp_term_moveto}
term-moveup {plisp_term_moveup}
term-movedown {plisp_term_movedown}
term-moveleft {plisp_term_moveleft}
term-moveright {plisp_term_moveright}
term-getlines {plisp_term_getlines}
term-getcols {plisp_term_getcols}
term-prinl {plisp_term_prinl}
term-getcx {plisp_term_getcx}
term-getcy {plisp_term_getcy}
term-getchar {plisp_term_getchar}
term-decode {plisp_term_decode}

### eLua ###
elua-version {plisp_elua_version}
elua-save-history {plisp_elua_save_history}
elua-shell {plisp_elua_shell}

### CPU ###
cpu-w32 {plisp_cpu_w32}
cpu-r32 {plisp_cpu_r32}
cpu-w16 {plisp_cpu_w16}
cpu-r16 {plisp_cpu_r16}
cpu-w8 {plisp_cpu_w8}
cpu-r8 {plisp_cpu_r8}
cpu-clock {plisp_cpu_clock}

### Timers ###
tmr-delay {tmr_delay}
tmr-read  {tmr_read}
tmr-start {tmr_start}
tmr-gettimediff {tmr_gettimediff}
tmr-getdiffnow {tmr_getdiffnow}
tmr-getmindelay {tmr_getmindelay}
tmr-getmaxdelay {tmr_getmaxdelay}
tmr-setclock {tmr_setclock}
tmr-getclock {tmr_getclock}
tmr-decode {tmr_decode}

### I2C ###
i2c-setup {plisp_i2c_setup}
i2c-start {plisp_i2c_start}
i2c-stop {plisp_i2c_stop}
i2c-address {plisp_i2c_address}
i2c-write {plisp_i2c_write}
i2c-read {plisp_i2c_read}

### PWM ###
pwm-setup {plisp_pwm_setup}
pwm-start {plisp_pwm_start}
pwm-stop {plisp_pwm_stop}
pwm-setclock {plisp_pwm_setclock}
pwm-getclock {plisp_pwm_getclock}

### SPI ###
spi-sson {plisp_spi_sson}
spi-ssoff {plisp_spi_ssoff}
spi-setup {plisp_spi_setup}
spi-write {plisp_spi_write}

### GPIO ###
pio-pin-setdir {plisp_pio_pin_setdir}
pio-pin-setpull {plisp_pio_pin_setpull}
pio-pin-setval {plisp_pio_pin_setval}
pio-pin-sethigh {plisp_pio_pin_sethigh}
pio-pin-setlow {plisp_pio_pin_setlow}
pio-pin-getval {plisp_pio_pin_getval}
pio-port-setdir {plisp_pio_port_setdir}
pio-port-setpull {plisp_pio_port_setpull}
pio-port-setval {plisp_pio_port_setval}
pio-port-sethigh {plisp_pio_port_sethigh}
pio-port-setlow {plisp_pio_port_setlow}
pio-port-getval {plisp_pio_port_getval}
pio-decode {plisp_pio_decode}

### UART ###
uart-setup {plisp_uart_setup}
uart-write {plisp_uart_write}
uart-set-buffer {plisp_uart_set_buffer}
uart-set-flow-control {plisp_uart_set_flow_control}
uart-getchar {plisp_uart_getchar}
uart-vuart-tmr-ident {plisp_uart_vuart_tmr_ident}
uart-read {plisp_uart_read}

### ADC ###
adc-maxval {plisp_adc_maxval}
adc-setclock {plisp_adc_setclock}
adc-isdone {plisp_adc_isdone}
adc-setblocking {plisp_adc_setblocking}
adc-setsmoothing {plisp_adc_setsmoothing}
adc-sample {plisp_adc_sample}
adc-getsample {plisp_adc_getsample}
adc-getsamples {plisp_adc_getsamples}
adc-insertsamples {plisp_adc_insertsamples}
