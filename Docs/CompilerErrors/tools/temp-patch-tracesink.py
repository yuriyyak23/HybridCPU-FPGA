from pathlib import Path
p=Path(r'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Diagnostics\TraceSink.cpp')
s=p.read_text(encoding='utf-8-sig')
s=s.replace('#include <limits>', '#include <limits>\n#include <filesystem>\n#include <fstream>')
s=s.replace('''\t\t{
\t\t\tStreamWriter writer = StreamWriter(filePath, append: true);
        
\t\t\t// Write header if file is empty
\t\t\tstd::shared_ptr<FileInfo> fileInfo = std::make_shared<FileInfo>(filePath);
\t\t\tif (!fileInfo->Exists || fileInfo->Length == 0)
''','''\t\t{
\t\t\tconst std::filesystem::path outputPath(filePath);
\t\t\tstd::error_code fileError;
\t\t\tconst bool fileExists = std::filesystem::exists(outputPath, fileError) && !fileError;
\t\t\tconst auto fileLength = fileExists ? std::filesystem::file_size(outputPath, fileError) : 0;
\t\t\tconst bool writeHeader = !fileExists || fileError || fileLength == 0;
\t\t\tstd::wofstream writer(outputPath, std::ios::out | std::ios::app);
\t\t\tif (!writer.is_open()) throw std::runtime_error("Unable to open trace file for append.");

\t\t\t// Write header if file is empty
\t\t\tif (writeHeader)
''')
s=s.replace('writer.WriteLine(L"PC,BundleId,OpIndex,Opcode,ExceptionFlags,ExceptionCount");','writer << L"PC,BundleId,OpIndex,Opcode,ExceptionFlags,ExceptionCount" << L\'\\n\';')
s=s.replace('writer.WriteLine(L"PC,BundleId,OpIndex,Opcode,Operands,PredicateMask,ExceptionFlags,Result,ExceptionCount");','writer << L"PC,BundleId,OpIndex,Opcode,Operands,PredicateMask,ExceptionFlags,Result,ExceptionCount" << L\'\\n\';')
s=s.replace('writer.WriteLine(std::format(L"{},{},{},{},{},{}", evt.PC, evt.BundleId, evt.OpIndex, evt.Opcode, evt.Flags.ToString(), evt.ExceptionCount));','writer << std::format(L"{},{},{},{},{},{}", evt.PC, evt.BundleId, evt.OpIndex, evt.Opcode, evt.Flags.ToString(), evt.ExceptionCount) << L\'\\n\';')
s=s.replace('writer.WriteLine(std::format(\n','writer << std::format(\n')
s=s.replace('evt.Flags.ToString(), result, evt.ExceptionCount));','evt.Flags.ToString(), result, evt.ExceptionCount) << L\'\\n\';')
s=s.replace('''\t\t{
\t\t\tStreamWriter writer = StreamWriter(filePath, append: true);
        
\t\t\tfor (auto evt : events)
''','''\t\t{
\t\t\tconst std::filesystem::path outputPath(filePath);
\t\t\tstd::wofstream writer(outputPath, std::ios::out | std::ios::app);
\t\t\tif (!writer.is_open()) throw std::runtime_error("Unable to open trace file for append.");

\t\t\tfor (auto evt : events)
''')
s=s.replace('writer.WriteLine(json);','writer << json << L\'\\n\';')
p.write_text(s,encoding='utf-8')
