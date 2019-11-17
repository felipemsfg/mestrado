import { Injectable } from '@angular/core';
import { Observable, Subscribable, BehaviorSubject, Subject } from 'rxjs';
import { tap  } from 'rxjs/operators';
import { HttpClient, HttpHeaders } from '@angular/common/http';
import { BaseService } from './base-service';
import { Patient } from '../models/patient';

const httpOptions = {
  headers: new HttpHeaders({ 'Content-Type': 'application/json', 'Cache-Control': 'no-cache' })
};

const httpOptionsFileUpload = {
  headers: new HttpHeaders({ 'Content-Type': 'multipart/form-data' })
};

@Injectable({
  providedIn: 'root'
})
export class PatientService  extends BaseService {
  controller = 'patient';

  constructor(public http: HttpClient) {
    super();
  }

  public Collection$: BehaviorSubject<Patient[]> = new BehaviorSubject(new Array<Patient>());
  public Entity$: BehaviorSubject<Patient> = new BehaviorSubject(new Patient());
  public ErrorToGet$: Subject<void> = new Subject();
  public ErrorToSave$: Subject<Patient> = new Subject();
  public ErrorToDelete$: Subject<void> = new Subject();
  public SalvarSuccess$: Subject<Patient> = new Subject<Patient>();
  public DeleteSuccess$: Subject<void> = new Subject<void>();


  Salvar(data: Patient) {
    this.http.post<any>(`${this.UrlService}${this.controller}`, data).subscribe(result => {
        this.SalvarSuccess$.next(result);
    }, err => {
        this.ErrorToSave$.next(data);
    });
  }


  GetAll() {
    this.http.get<Patient[]>(`${this.UrlService}${this.controller}`).subscribe(result => {
        this.Collection$.next(result);
    }, err => {
        this.ErrorToGet$.next();
    });
  }

  GetById(id: string) {
    return this.http.get<Patient>(`${this.UrlService}${this.controller}/${id}`).subscribe(data => {
        this.Entity$.next(data);
    }, err => {
        this.ErrorToGet$.next();
    });
  }

  Delete(id: string) {
    return this.http.delete(`${this.UrlService}${this.controller}/${id}`).subscribe(result => {
        this.DeleteSuccess$.next();
    }, err => {
        this.ErrorToDelete$.next();
    });
  }
}
