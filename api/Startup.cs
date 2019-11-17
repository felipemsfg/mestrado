using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Mestrado.Repository;
using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Hosting;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;

namespace Mestrado
{
    public class Startup
    {
        private const string MongoDbSectionName = "MongoDb";
        private const string MongoDbSectionConnectionStringValueName = "ConnectionString";
        private const string MongoDbSectionDatabaseValueName = "Database";

        public Startup(IConfiguration configuration)
        {
            Configuration = configuration;
        }

        public IConfiguration Configuration { get; }

        // This method gets called by the runtime. Use this method to add services to the container.
        public void ConfigureServices(IServiceCollection services)
        {
            services.AddCors();
            services.AddMvc().SetCompatibilityVersion(CompatibilityVersion.Version_2_2);
            services.AddScoped<MongoDbClient>(s => new MongoDbClient(Configuration.GetSection(MongoDbSectionName)[MongoDbSectionConnectionStringValueName], Configuration.GetSection(MongoDbSectionName)[MongoDbSectionDatabaseValueName]));
            services.AddScoped<IBase<Entities.Paciente>>(s => new Base<Entities.Paciente>(s.GetService<MongoDbClient>(), "Paciente"));
            services.AddTransient<Services.Patient, Services.Patient>();
        }

        // This method gets called by the runtime. Use this method to configure the HTTP request pipeline.
        public void Configure(IApplicationBuilder app, IHostingEnvironment env)
        {
            app.UseCors(builder => builder.AllowAnyOrigin().AllowAnyHeader().AllowAnyMethod().AllowCredentials());
            if (env.IsDevelopment())
            {
                app.UseDeveloperExceptionPage();
            }

            app.UseMvc();
        }
    }
}
