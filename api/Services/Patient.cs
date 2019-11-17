using System;
using System.Collections.Generic;
using System.Data;
using System.IO;
using System.Linq;
using Mestrado.Repository;
using Microsoft.EntityFrameworkCore.SqlServer.Query.ExpressionTranslators.Internal;
using Microsoft.Office.Interop.Excel;
using OfficeOpenXml;

namespace Mestrado.Services
{
    public class Patient
    {
        private readonly IBase<Entities.Paciente> _repository;

        public Patient(IBase<Entities.Paciente> repository)
        {
            _repository = repository;
        }

        public void Save(Entities.Paciente entity)
        {
            _repository.AddOrReplace(entity);
        }

        public void Delete(Guid id) => _repository.Delete(id);

        public Entities.Paciente GetById(Guid id) => _repository.GetById(id);

        public List<Entities.Paciente> GetAll() => _repository.GetAll().ToList();

        public void PrintAllWithImageAndSendPdfToEmail(string path)
        {
            using (ExcelPackage excel = new ExcelPackage())
            {
                excel.Workbook.Worksheets.Add("Worksheet1");
                var worksheet = excel.Workbook.Worksheets["Worksheet1"];

                var content = GetAll();

                var headerRow = new List<string[]>()
                {
                    new string[] { "Patient Id", "Data do Exame", "MTA Direito", "MTA Esquerdo" }
                };

                //var contentRows = new List<string[]>()
                //{
                //    new string[] {"1", "Felipe", "Marques", "123"},
                //    new string[] {"2", "Lidiane", "Lima", "456"}
                //};

                headerRow.ForEach(row =>
                {
                    // Determine the header range (e.g. A1:D1)
                    var headerRange = "A1:" + char.ConvertFromUtf32(row.Length + 64) + "1";

                    // Popular header row data
                    worksheet.Cells[headerRange].LoadFromArrays(new List<string[]>{row.ToArray()});
                    worksheet.Cells[headerRange].Style.Font.Bold = true;
                    worksheet.Cells[headerRange].Style.Font.Size = 12;
                    worksheet.Cells[headerRange].Style.Font.Color.SetColor(System.Drawing.Color.Black);
                });

                for (var temp = 0; temp < content.Count; temp++)
                {
                    var headerRange = $"A{temp+2}:{char.ConvertFromUtf32(headerRow[0].Length + 64)}{temp+2}";

                    // Popular header row data
                    worksheet.Cells[headerRange].LoadFromArrays(new List<object[]> { new object[] { content[temp].PatientId, DateTime.Parse(content[temp].ImageDate), content[temp].MtaRight, content[temp].MtaLeft } });
                    worksheet.Cells[$"B{temp+2}"].Style.Numberformat.Format = "yyyy-mm-dd";
                    worksheet.Cells[headerRange].Style.Font.Color.SetColor(System.Drawing.Color.Black);
                    worksheet.Cells[headerRange].Style.Font.Size = 11;
                }


                FileInfo excelFile = new FileInfo($@"C:\Users\felipe\Documents\{DateTime.Now:yyyy-MM-dd HH-mm-ss}.xlsx");
                excel.SaveAs(excelFile);
            }
        }

        public void PrintAllWithoutImageAndSendPdfToEmail()
        {

        }
    }
}