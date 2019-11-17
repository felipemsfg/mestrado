using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Mestrado.Entities;

// ReSharper disable TypeParameterCanBeVariant

namespace Mestrado.Repository
{
    public interface IBase<TOne> : IBaseAddRepository<TOne>, IBaseDeleteRepository<TOne>, IBaseGetAllRepository<TOne>, IBaseGetByIdRepository<TOne> where TOne : class, IEntity
    {

    }

    public interface IBaseAddRepository<TOne> where TOne : class
    {
        Task<bool> AddOrReplace(TOne entity);

    }

    public interface IBaseDeleteRepository<TOne> where TOne : class
    {
        void Delete(Guid id);
    }

    public interface IBaseGetAllRepository<TOne> where TOne : class
    {
        IEnumerable<TOne> GetAll();

    }

    public interface IBaseGetByIdRepository<TOne> where TOne : class
    {
        TOne GetById(Guid id);
    }
}