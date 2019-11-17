using System;
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Hosting;
using Microsoft.AspNetCore.Mvc;

namespace Mestrado.Controllers
{
    [Route("patient")]
    [ApiController]
    public class PatientController : ControllerBase
    {
        private readonly Services.Patient _service;
        private readonly IHostingEnvironment _env;

        public PatientController(Services.Patient service, IHostingEnvironment env)
        {
            _service = service;
            _env = env;
        }

        [HttpPost]
        [AllowAnonymous]
        public ActionResult CreateNew([FromBody] Entities.Paciente entity)
        {
            if (entity.Id == Guid.Empty) entity.Id = Guid.NewGuid();
            _service.Save(entity);
            return Ok(entity);
        }

        [HttpPut]
        [AllowAnonymous]
        public IActionResult Update([FromBody]Entities.Paciente entity)
        {
            _service.Save(entity);
            return Ok(entity);
        }

        [HttpDelete("{id}")]
        [AllowAnonymous]
        public IActionResult Delete(Guid id)
        {
            _service.Delete(id);
            return Ok();
        }

        [HttpGet("{id}")]
        [AllowAnonymous]
        public IActionResult GetById(Guid id)
        {
            return Ok(_service.GetById(id));
        }

        [HttpGet]
        [AllowAnonymous]
        public IActionResult GetAll()
        {
            return Ok(_service.GetAll());
        }

        [HttpGet("export-excel")]
        [AllowAnonymous]
        public IActionResult ExportExcel()
        {
            string contentRootPath = _env.ContentRootPath;
            string webRootPath = _env.WebRootPath;
            _service.PrintAllWithImageAndSendPdfToEmail(_env.ContentRootPath);
            return Ok();
        }

        [HttpPut("print-all-with-image")]
        [AllowAnonymous]
        public IActionResult PrintAllWithImageAndSendPdfToEmail()
        {
            string contentRootPath = _env.ContentRootPath;
            string webRootPath = _env.WebRootPath;
            _service.PrintAllWithImageAndSendPdfToEmail(_env.ContentRootPath);
            return Ok();
        }
        [HttpPut("print-all-without-image")]
        [AllowAnonymous]
        public IActionResult PrintAllWithoutImageAndSendPdfToEmail()
        {
            _service.PrintAllWithoutImageAndSendPdfToEmail();
            return Ok();
        }
    }
}