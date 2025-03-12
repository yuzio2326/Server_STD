@echo off
rem echo HeaderParser.bat Start: %cd%
rem %1: $(SolutionDir)
rem %2: [project.Name]
rem %3: [project.SourceRootPath]
rem %4: %EngineDir%

cd /D %1
rem Intermediate
cd ..
rem FrontendSlnDir
cd ..
rem echo %cd%

rem @echo [   -   -   -   - Header parser -   -   -   -   ]

if not exist Intermediate\HeaderParser\ (
@echo - mkdir Intermediate\HeaderParser
rem Intermediate에 HeaderParser 디렉토리를 만듭니다
mkdir Intermediate\HeaderParser
)

if not exist Intermediate\HeaderParser\.git (
rem Intermediate\HeaderParser에 git 설정이 없다면, 변경된 파일 감지를 위한 git을 초기화 합니다
@echo - HeaderParser git init
pushd Intermediate\HeaderParser

rem HeaderParserTarget.txt 파일을 gitignore에 추가합니다
echo HeaderParserTarget.txt >> .gitignore
rem 자동으로 생성될 모든 generated.h 파일을 gitignore에 추가합니다
echo HeaderParserGenerated/ >> .gitignore
echo *.generated.h >> .gitignore
rem 위 내용을 commit 합니다
"C:\Program Files\Git\bin\sh.exe" --login -c "git init && git add . && git commit -m "gitignore""
rem @echo - [Done] git init

popd
)

rem cd
rem @echo - [Start] copy headers %3
rem @echo.

if not exist Intermediate\HeaderParser\%2\ (
mkdir Intermediate\HeaderParser\%2
)
rem 해당 프로젝트 해더 파일들 복사
for /r %3 %%f in (*.h) do xcopy "%%f" "Intermediate\HeaderParser\%2" /D /Y /Q > nul 2>&1

rem @echo - [Done] copy headers
rem @echo.

pushd Intermediate\HeaderParser
rem 파싱할 모든 header파일(이전 commit과 비교해서 바뀐 header만) 이름을 수집해서 HeaderParserTarget.txt에 저장하고, 모든 내용(해더)을 commit 합니다.
"C:\Program Files\Git\bin\sh.exe" --login -c "git status --porcelain -uall | cut -c 1-3 --complement | egrep .h$ > HeaderParserTarget.txt && git add . && git commit -m "NewHeaders""
popd

pushd Intermediate\HeaderParser
rem 수집된 변경된 header파일 이름을 header-parser에 전달하고 파싱합니다.
rem 파싱된 파일은 name.generated.h로 Intermediate\HeaderParser 폴더에 복사된 header위치에 생성됩니다.
start /b "HeaderParser" "%~4\Engine\Source\Programs\HeaderParser\out\Release\header-parser.exe" HeaderParserTarget.txt -c UCLASS -e UENUM -f UFUNC -p UPROPERTY
popd

rem @echo [   -   -   -   - Header parser Done -   -   -   -   ]